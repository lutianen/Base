///
/// @brief Logger
/// @note Default async logger
/// @usage
///     #include <Base/logger.h>
///     int main {
///         initLogger(Lute::Logger::LogLevel::TRACE);
///         LOG_WARN << "This is log WARN TEST";
///         ...
///     };
///

#pragma once

#include <Base/fsUtils.h>    // AppendFile
#include <Base/mutex.h>      // MutexLock
#include <Base/thread.h>     // Thread
#include <Base/timestamp.h>  // Timestamp
#include <Base/utils.h>      // memZero

#include <memory>  // unique_ptr

/// NOTE Key Definitions
/// The prefix of logfile
#define LOG_FILE_NAME "SYSTEM"
/// The rollSize of logfile
#define LOG_FILE_ROLLSIZE 4 * 1024 * 1024
/// The flush interval of logfile
#define LOG_FLUSH_INTERVAL 3
/// Message Delimiter
constexpr char MsgDelimiter[] = "@ ";

namespace Lute {

namespace detail {
    const int kSmallBuffer = 4000;
    const int kLargeBuffer = 4000 * 1000;

    template <int SIZE>
    class FixedBuffer {
    public:
        /// non-copyable
        FixedBuffer(const FixedBuffer&) = delete;
        FixedBuffer& operator=(FixedBuffer&) = delete;

        FixedBuffer() : cur_(data_) { setCookie(cookieStart); }
        ~FixedBuffer() { setCookie(cookieEnd); }

        void append(const char* /*restrict*/ buf, size_t len) {
            // FIXME: append partially
            if (static_cast<size_t>(avail()) > len) {
                ::memcpy(cur_, buf, len);
                cur_ += len;
            }
        }

        const char* data() const { return data_; }
        int length() const { return static_cast<int>(cur_ - data_); }

        // write to data_ directly
        char* current() { return cur_; }
        int avail() const { return static_cast<int>(end() - cur_); }
        void add(size_t len) { cur_ += len; }

        void reset() { cur_ = data_; }
        void bzero() { memZero(data_, sizeof data_); }

        // for used by GDB
        const char* debugString();
        void setCookie(void (*cookie)()) { cookie_ = cookie; }
        // for used by unit test
        std::string toString() const { return std::string(data_, length()); }
        // StringPiece toStringPiece() const {
        // return StringPiece(data_, static_cast<size_t>(length()));
        // }

    private:
        const char* end() const { return data_ + sizeof data_; }
        // Must be outline function for cookies.
        static void cookieStart();
        static void cookieEnd();

        void (*cookie_)();
        char data_[SIZE];
        char* cur_;
    };
}  // namespace detail

// ---

class LogStream {
public:
    using Buffer = detail::FixedBuffer<detail::kSmallBuffer>;
    using self = LogStream;
    static const int kMaxNumericSize = 48;

    /// non-copyable
    LogStream(const LogStream&) = delete;
    LogStream& operator=(LogStream&) = delete;

    LogStream() = default;

    self& operator<<(bool v);
    self& operator<<(short);
    self& operator<<(unsigned short);
    self& operator<<(int);
    self& operator<<(unsigned int);
    self& operator<<(long);
    self& operator<<(unsigned long);
    self& operator<<(long long);
    self& operator<<(unsigned long long);
    self& operator<<(const void*);
    self& operator<<(float v);
    self& operator<<(double);
    // self& operator<<(long double);
    self& operator<<(char v);
    // self& operator<<(signed char);
    // self& operator<<(unsigned char);
    self& operator<<(const char* str);
    self& operator<<(const unsigned char* str);
    self& operator<<(const std::string& v);

    // self& operator<<(const StringPiece& v) {
    //     buffer_.append(v.data(), static_cast<size_t>(v.size()));
    //     return *this;
    // }

    self& operator<<(const Buffer& v);

    void append(const char* data, int len) {
        buffer_.append(data, static_cast<size_t>(len));
    }
    const Buffer& buffer() const { return buffer_; }
    void resetBuffer() { buffer_.reset(); }

private:
    void staticCheck();

    template <typename T>
    void formatInteger(T);

    Buffer buffer_;
};

class Fmt {
public:
    ///
    Fmt(const Fmt&) = delete;
    Fmt& operator=(Fmt&) = delete;

    template <typename T>
    Fmt(const char* fmt, T val);

    const char* data() const { return buf_; }
    int length() const { return length_; }

private:
    char buf_[32];
    int length_;
};

///
/// @brief
///
class LogFile {
public:
    /// non - copyable
    LogFile(const LogFile&) = delete;
    LogFile& operator=(LogFile&) = delete;

    LogFile(const std::string& basename, off_t rollSize, bool threadSafe = true,
            int flushInterval = 3, int checkEveryN = 1024);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    bool rollFile();

private:
    const static int kRollPerSeconds_ = 60 * 60 * 24;

    void append_unlocked(const char* logline, int len);

    static std::string getLogFileName(const std::string& basename, time_t* now);

    const std::string basename_;  // 日志文件名
    const off_t rollSize_;        // 日志文件 roll threshold
    const int flushInterval_;     // 日志写入间隔
    const int checkEveryN_;       // check every N

    int count_;

    std::unique_ptr<MutexLock> mutex_;
    time_t startOfPeriod_;              // 开始记录日志的时间
    time_t lastRoll_;                   // Last roll time
    time_t lastFlush_;                  // Last flush time
    std::unique_ptr<AppendFile> file_;  // 日志文件
};

///
/// @brief
///
class Logger {
public:
    enum class LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    using OutputFunc = void (*)(const char* msg, int len);
    using FlushFunc = void (*)();

    /// @brief compile time calculation of basename of source file
    class SourceFile {
    public:
        template <int N>
        SourceFile(const char (&arr)[N]) : data_(arr), size_(N - 1) {
            const char* slash = ::strrchr(data_, '/');
            if (slash) {
                data_ = slash + 1;
                size_ -= static_cast<int>(data_ - arr);
            }
        }

        explicit SourceFile(const char* filename) : data_(filename) {
            const char* slash = ::strrchr(filename, '/');
            if (slash) {
                data_ = slash + 1;
            }
            size_ = static_cast<int>(::strlen(data_));
        }

        const char* data_;
        int size_;
    };

    /// @brief Constructor and Destructor
    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream& stream() { return impl_.stream_; }

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);

private:
    class Impl {
    public:
        using LogLevel = Logger::LogLevel;

        /// @brief Constructor
        Impl(LogLevel level, int old_errno, const SourceFile& file, int line);

        /**
         * @brief 将本地时间格式化
         */
        void formatTime();

        Timestamp time_;
        LogStream stream_;
        LogLevel level_;
        int line_;
        SourceFile basename_;
    };

    Impl impl_;
};

///
/// @brief AsyncLogger
///
class AsyncLogger {
public:
    /// non-copyable
    AsyncLogger(const AsyncLogger&) = delete;
    AsyncLogger& operator=(AsyncLogger&) = delete;

    AsyncLogger(const std::string& basename, off_t rollSize,
                int flushInterval = 3);
    ~AsyncLogger() {
        if (running_) stop();
    }

    void append(const char* logline, int len);

    /// @brief start -
    ///  1. start thread
    ///  2. latch wait
    void start() {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop() NO_THREAD_SAFETY_ANALYSIS {
        running_ = false;
        cond_.notify();
        thread_.join();
    }

private:
    void threadFunc();

    using Buffer = detail::FixedBuffer<detail::kLargeBuffer>;
    using BufferVector = std::vector<std::unique_ptr<Buffer>>;
    using BufferPtr = BufferVector::value_type;

    const int flushInterval_;
    std::atomic<bool> running_;
    const std::string basename_;
    const off_t rollSize_;
    Thread thread_;
    CountDownLatch latch_;
    MutexLock mutex_;
    Condition cond_ GUARDED_BY(mutex_);

    /// 当前缓冲
    BufferPtr currentBuffer_ GUARDED_BY(mutex_);
    /// 预备缓冲，相当于 currentBuffer_ 的“备胎”
    BufferPtr nextBuffer_ GUARDED_BY(mutex_);
    /// 待写入文件的已填满的缓冲
    BufferVector buffers_ GUARDED_BY(mutex_);
};

}  // namespace Lute

// helper class for known string length at compile time
class T {
public:
    T(const char* str, unsigned len) : str_(str), len_(len) {
        assert(strlen(str) == len_);
    }

    const char* str_;
    const unsigned len_;
};

///
/// @brief Not declared in class LogStream
///
inline Lute::LogStream& operator<<(Lute::LogStream& s, T v) {
    s.append(v.str_, static_cast<int>(v.len_));
    return s;
}
///
/// @brief Not declared in class LogStream
///
inline Lute::LogStream& operator<<(Lute::LogStream& s,
                                   const Lute::Logger::SourceFile& v) {
    s.append(v.data_, v.size_);
    return s;
}
///
/// @brief Not declared in class LogStream
///
inline Lute::LogStream& operator<<(Lute::LogStream& s, const Lute::Fmt& fmt) {
    s.append(fmt.data(), fmt.length());
    return s;
}

// NOTE Global logger level
extern Lute::Logger::LogLevel g_logLevel;
inline Lute::Logger::LogLevel Lute::Logger::logLevel() { return g_logLevel; }

#define LOG_TRACE                                                             \
    if (Lute::Logger::logLevel() <= Lute::Logger::LogLevel::TRACE)            \
    Lute::Logger(__FILE__, __LINE__, Lute::Logger::LogLevel::TRACE, __func__) \
        .stream()
#define LOG_DEBUG                                                             \
    if (Lute::Logger::logLevel() <= Lute::Logger::LogLevel::DEBUG)            \
    Lute::Logger(__FILE__, __LINE__, Lute::Logger::LogLevel::DEBUG, __func__) \
        .stream()
#define LOG_INFO                                                  \
    if (Lute::Logger::logLevel() <= Lute::Logger::LogLevel::INFO) \
    Lute::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN \
    Lute::Logger(__FILE__, __LINE__, Lute::Logger::LogLevel::WARN).stream()
#define LOG_ERROR \
    Lute::Logger(__FILE__, __LINE__, Lute::Logger::LogLevel::ERROR).stream()
#define LOG_FATAL \
    Lute::Logger(__FILE__, __LINE__, Lute::Logger::LogLevel::FATAL).stream()
#define LOG_SYSERR Lute::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL Lute::Logger(__FILE__, __LINE__, true).stream()

inline const char* strerror_tl(int savedErrno);

// Taken from glog/logging.h
//
// Check that the input is non NULL.  This very useful in constructor
// initializer lists.
#define CHECK_NOTNULL(val) \
    Lute::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

// A small helper for CHECK_NOTNULL().
template <typename T>
T* CheckNotNull(Lute::Logger::SourceFile file, int line, const char* names,
                T* ptr) {
    if (ptr == NULL) {
        Lute::Logger(file, line, Lute::Logger::LogLevel::FATAL).stream()
            << names;
    }
    return ptr;
}

extern std::shared_ptr<Lute::AsyncLogger> g_asyncLogger;
inline void defaultAsyncOutput(const char* msg, int len) {
    g_asyncLogger->append(msg, len);
}

///
/// @brief Initialize logger
/// @note default log level is INFO
///       default output is defaultAsyncOutput
/// @param logLevel The logLevel to be set
///
inline void initLogger(
    Lute::Logger::LogLevel logLevel = Lute::Logger::LogLevel::INFO) {
    Lute::Logger::setLogLevel(logLevel);
    Lute::Logger::setOutput(defaultAsyncOutput);
    g_asyncLogger->start();
}
