#include <Base/logger.h>
#include <Base/sigleton.h>
#include <Base/utils.h>

Lute::Logger::LogLevel initLogLevel();  // forward declaration

/**
 * @brief Default output Func.
 *  It push `msg` to `stdout`.
 * @param msg Message
 */
void defaultOutput(const char* msg, int len) {
    size_t n = ::fwrite(msg, 1, static_cast<size_t>(len), stdout);

    assert(n == static_cast<size_t>(len));
    if (n != static_cast<size_t>(len)) {
        char _buf[128] = {};
        ::snprintf(_buf, sizeof(_buf), "%s : %d - n should be equal len!!!",
                   __FILE__, __LINE__);
        ::perror(_buf);
    }
}
void defaultFlush() { ::fflush(stdout); }

auto g_asyncLogger = Lute::SingletonPtr<Lute::AsyncLogger>::GetInstance(
    LOG_FILE_NAME, LOG_FILE_ROLLSIZE, LOG_FLUSH_INTERVAL);

/// NOTE Global Outuput/Flush Function
Lute::Logger::OutputFunc g_output = defaultOutput;
Lute::Logger::FlushFunc g_flush = defaultFlush;

__thread char t_errnobuf[512];
__thread char t_time[64];
__thread time_t t_lastSecond;

/// NOTE Global logger level is set
Lute::Logger::LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[static_cast<unsigned int>(
    Lute::Logger::LogLevel::NUM_LOG_LEVELS)] = {"TRACE ", "DEBUG ", "INFO  ",
                                                "WARN  ", "ERROR ", "FATAL "};
constexpr int LogLevelStrLen = sizeof(LogLevelName) / sizeof(LogLevelName[0]);

///
/// @brief Get a string that describes the error code
///
/// @param savedErrno Error code
/// @return const char* String that describes the error code
///
const char* strerror_tl(int savedErrno) {
    return ::strerror_r(savedErrno, t_errnobuf, sizeof(t_errnobuf));
}

inline void defaultAsyncOutput(const char* msg, int len) {
    g_asyncLogger->append(msg, len);
}

void initLogger(Lute::Logger::LogLevel logLevel) {
    Lute::Logger::setLogLevel(logLevel);
    Lute::Logger::setOutput(defaultAsyncOutput);
    g_asyncLogger->start();
}

/// NOTE ----------- FixedBuffer -----------
template <int SIZE>
const char* Lute::detail::FixedBuffer<SIZE>::debugString() {
    *cur_ = '\0';
    return data_;
}

template <int SIZE>
void Lute::detail::FixedBuffer<SIZE>::cookieStart() {}

template <int SIZE>
void Lute::detail::FixedBuffer<SIZE>::cookieEnd() {}

/// NOTE ----------- LogStream -----------
void Lute::LogStream::staticCheck() {
    static_assert(kMaxNumericSize - 10 > std::numeric_limits<double>::digits10,
                  "kMaxNumericSize is large enough");
    static_assert(
        kMaxNumericSize - 10 > std::numeric_limits<long double>::digits10,
        "kMaxNumericSize is large enough");
    static_assert(kMaxNumericSize - 10 > std::numeric_limits<long>::digits10,
                  "kMaxNumericSize is large enough");
    static_assert(
        kMaxNumericSize - 10 > std::numeric_limits<long long>::digits10,
        "kMaxNumericSize is large enough");
}

template <typename T>
void Lute::LogStream::formatInteger(T v) {
    if (buffer_.avail() >= kMaxNumericSize) {
        size_t len = integer2Str(buffer_.current(), v);
        buffer_.add(len);
    }
}

Lute::LogStream& Lute::LogStream::operator<<(short v) {
    *this << static_cast<int>(v);
    return *this;
}

Lute::LogStream& Lute::LogStream::operator<<(unsigned short v) {
    *this << static_cast<unsigned int>(v);
    return *this;
}

Lute::LogStream& Lute::LogStream::operator<<(int v) {
    formatInteger(v);
    return *this;
}

Lute::LogStream& Lute::LogStream::operator<<(unsigned int v) {
    formatInteger(v);
    return *this;
}

Lute::LogStream& Lute::LogStream::operator<<(long v) {
    formatInteger(v);
    return *this;
}

Lute::LogStream& Lute::LogStream::operator<<(unsigned long v) {
    formatInteger(v);
    return *this;
}

Lute::LogStream& Lute::LogStream::operator<<(long long v) {
    formatInteger(v);
    return *this;
}

Lute::LogStream& Lute::LogStream::operator<<(unsigned long long v) {
    formatInteger(v);
    return *this;
}

Lute::LogStream& Lute::LogStream::operator<<(const void* p) {
    auto v = reinterpret_cast<uintptr_t>(p);
    if (buffer_.avail() >= kMaxNumericSize) {
        char* buf = buffer_.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = integer2StrHex(buf + 2, v);
        buffer_.add(len + 2);
    }
    return *this;
}

// FIXME: replace this with Grisu3 by Florian Loitsch.
Lute::LogStream& Lute::LogStream::operator<<(double v) {
    if (buffer_.avail() >= kMaxNumericSize) {
        int len = ::snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
        buffer_.add(static_cast<size_t>(len));
    }
    return *this;
}
Lute::LogStream::self& Lute::LogStream::operator<<(bool v) {
    buffer_.append(v ? "1" : "0", 1);
    return *this;
}
Lute::LogStream::self& Lute::LogStream::operator<<(float v) {
    *this << static_cast<double>(v);
    return *this;
}
Lute::LogStream::self& Lute::LogStream::operator<<(char v) {
    buffer_.append(&v, 1);
    return *this;
}
Lute::LogStream::self& Lute::LogStream::operator<<(const char* str) {
    if (str) {
        buffer_.append(str, strlen(str));
    } else {
        buffer_.append("(null)", 6);
    }
    return *this;
}

Lute::LogStream::self& Lute::LogStream::operator<<(const unsigned char* str) {
    return operator<<(reinterpret_cast<const char*>(str));
}

Lute::LogStream::self& Lute::LogStream::operator<<(const std::string& v) {
    buffer_.append(v.c_str(), v.size());
    return *this;
}

Lute::LogStream::self& Lute::LogStream::operator<<(
    const Lute::LogStream::Buffer& v) {
    // *this << v.toStringPiece();
    *this << v.toString();
    return *this;
}

/// NOTE ----------- Fmt -----------
template <typename T>
Lute::Fmt::Fmt(const char* fmt, T val) {
    static_assert(std::is_arithmetic<T>::value == true,
                  "Must be arithmetic type");

    length_ = ::snprintf(buf_, sizeof(buf_), fmt, val);
    assert(static_cast<size_t>(length_) < sizeof(buf_));
}

// Explicit instantiations
template Lute::Fmt::Fmt(const char* fmt, char);
template Lute::Fmt::Fmt(const char* fmt, short);
template Lute::Fmt::Fmt(const char* fmt, unsigned short);
template Lute::Fmt::Fmt(const char* fmt, int);
template Lute::Fmt::Fmt(const char* fmt, unsigned int);
template Lute::Fmt::Fmt(const char* fmt, long);
template Lute::Fmt::Fmt(const char* fmt, unsigned long);
template Lute::Fmt::Fmt(const char* fmt, long long);
template Lute::Fmt::Fmt(const char* fmt, unsigned long long);
template Lute::Fmt::Fmt(const char* fmt, float);
template Lute::Fmt::Fmt(const char* fmt, double);

/// NOTE ----------- LogFile -----------
Lute::LogFile::LogFile(const std::string& basename, off_t rollSize,
                       bool threadSafe, int flushInterval, int checkEveryN)
    : basename_(basename),
      rollSize_(rollSize),
      flushInterval_(flushInterval),
      checkEveryN_(checkEveryN),
      count_(0),
      mutex_(threadSafe ? new MutexLock : nullptr),
      startOfPeriod_(0),
      lastRoll_(0),
      lastFlush_(0) {
    assert(basename.find('/') == std::string::npos);
    rollFile();
}

Lute::LogFile::~LogFile() = default;

void Lute::LogFile::append(const char* logline, int len) {
    if (mutex_) {
        MutexLockGuard lock(*mutex_);
        append_unlocked(logline, len);
    } else {
        append_unlocked(logline, len);
    }
}

void Lute::LogFile::flush() {
    if (mutex_) {
        MutexLockGuard lock(*mutex_);
        file_->flush();
    } else {
        file_->flush();
    }
}

void Lute::LogFile::append_unlocked(const char* logline, int len) {
    file_->append(logline, len);

    if (file_->writtenBytes() > rollSize_) {
        rollFile();
    } else {
        ++count_;
        if (count_ >= checkEveryN_) {
            count_ = 0;
            time_t now = ::time(nullptr);
            time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
            if (thisPeriod_ != startOfPeriod_) {
                rollFile();
            } else if (now - lastFlush_ > flushInterval_) {
                lastFlush_ = now;
                file_->flush();
            }
        }
    }
}

bool Lute::LogFile::rollFile() {
    time_t now = 0;
    std::string filename = getLogFileName(basename_, &now);
    time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

    if (now > lastRoll_) {
        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = start;
        file_.reset(new AppendFile(filename));
        return true;
    }
    return false;
}

/**
 * @brief Get the fileName of log.
 * E.g  basename.YYYYmmdd-HHMMSS.HOSTNAME.PID.log
 */
std::string Lute::LogFile::getLogFileName(const std::string& basename,
                                          time_t* now) {
    std::string filename;
    filename.reserve(basename.size() + 64);

    // basename
    filename = basename;

    // .YYYYmmdd-HHMMSS.
    char timebuf[32];
    struct tm tm {};
    *now = time(nullptr);
    localtime_r(now, &tm);
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;

    // hostname
    filename += ProcessInfo::hostname();

    // .pid
    char pidbuf[32];
    snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
    filename += pidbuf;

    // Suffix: .log
    filename += ".log";

    return filename;
}

/// NOTE ----------- Logger -----------
Lute::Logger::LogLevel initLogLevel() {
    if (::getenv("Lute_LOG_TRACE"))
        return Lute::Logger::LogLevel::TRACE;
    else if (::getenv("Lute_LOG_DEBUG"))
        return Lute::Logger::LogLevel::DEBUG;
    else
        return Lute::Logger::LogLevel::INFO;
}

///
/// @brief Add time, tid, logLevel, file:line to stream
///
Lute::Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file,
                         int line)
    : time_(Timestamp::now()),
      stream_(),
      level_(level),
      line_(line),
      basename_(file) {
    // Add time to stream
    formatTime();
    // Add tid to stream
    CurrentThread::tid();
    stream_ << T(CurrentThread::tidString(),
                 static_cast<unsigned int>(CurrentThread::tidStringLength()));
    // Add logLevel to stream
    stream_ << T(LogLevelName[static_cast<unsigned int>(level)],
                 LogLevelStrLen);
    // Add file:line to stream
    stream_ << basename_ << ':' << line_ << ' ';

    if (savedErrno != 0)
        stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
}

///
/// @brief Add time("YYYY/MM/DD hh:mm:ss ") to stream
///
void Lute::Logger::Impl::formatTime() {
    int64_t secondsSinceEpoch = time_.secondsSinceEpoch();
    if (secondsSinceEpoch != t_lastSecond) {
        t_lastSecond = secondsSinceEpoch;
        struct tm tm_time {};
        ::gmtime_r(&secondsSinceEpoch, &tm_time);

        int len = snprintf(
            t_time, sizeof(t_time), "%4d/%02d/%02d %02d:%02d:%02d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        assert(len == 19);
        (void)len;
    }
    stream_ << T(t_time, 19);
    stream_ << T(" ", 1);
}

Lute::Logger::Logger(SourceFile file, int line)
    : impl_(LogLevel::INFO, 0, file, line) {
    impl_.stream_ << MsgDelimiter;
}

Lute::Logger::Logger(SourceFile file, int line, LogLevel level,
                     const char* func)
    : impl_(level, 0, file, line) {
    impl_.stream_ << func << " " << MsgDelimiter;
}

Lute::Logger::Logger(SourceFile file, int line, LogLevel level)
    : impl_(level, 0, file, line) {
    impl_.stream_ << MsgDelimiter;
}

Lute::Logger::Logger(SourceFile file, int line, bool toAbort)
    : impl_(toAbort ? LogLevel::FATAL : LogLevel::ERROR, errno, file, line) {
    impl_.stream_ << MsgDelimiter;
}

Lute::Logger::~Logger() {
    impl_.stream_ << "\n";
    const LogStream::Buffer& buf(stream().buffer());
    g_output(buf.data(), buf.length());
    if (impl_.level_ == LogLevel::FATAL) {
        g_flush();
        abort();
    }
}

void Lute::Logger::setLogLevel(Logger::LogLevel level) { g_logLevel = level; }

void Lute::Logger::setOutput(OutputFunc out) { g_output = out; }

void Lute::Logger::setFlush(FlushFunc flush) { g_flush = flush; }

/// NOTE ----------- AsyncLogger -----------
Lute::AsyncLogger::AsyncLogger(const std::string& basename, off_t rollSize,
                               int flushInterval)
    : flushInterval_(flushInterval),
      running_(false),
      basename_(basename),
      rollSize_(rollSize),
      thread_(std::bind(&AsyncLogger::threadFunc, this), "AsyncLogger"),
      latch_(1),
      mutex_(),
      cond_(mutex_),
      currentBuffer_(new Buffer),
      nextBuffer_(new Buffer),
      buffers_() {
    currentBuffer_->bzero();
    nextBuffer_->bzero();

    // vector 的reserve增加了vector的capacity，但是它的size没有改变
    // 而resize改变了vector的capacity同时也增加了它的size
    buffers_.reserve(16);
}

/// @brief 前端线程调用，把日志信息放入缓冲
/// @param logline 日志信息
/// @param len 日志信息长度
void Lute::AsyncLogger::append(const char* logline, int len) {
    MutexLockGuard lock(mutex_);

    /// 当前写缓冲有足够的空间放置日志信息
    /// 直接放入
    if (currentBuffer_->avail() > len) {
        currentBuffer_->append(logline, static_cast<size_t>(len));
    } else {  /// 当前缓冲空间不足
        /// 将当前缓冲移动到 buffers_ 集合中，等待写入文件系统
        buffers_.push_back(std::move(currentBuffer_));  /// 利用 移动 而非 复制

        /// 如果 预备缓冲 未被移动，则将预备缓冲移动做到当前缓冲
        /// 也就是说，前端线程的写入速度小于后端线程的文件写入速度
        if (nextBuffer_) {
            currentBuffer_ = std::move(nextBuffer_);
        } else {  /// 前端线程写入太快，需要重新申请一块新的缓冲作为当前缓冲
            // Rarely happens
            currentBuffer_.reset(new Buffer);
        }

        /// 日志文件写入
        currentBuffer_->append(logline, static_cast<size_t>(len));
        cond_.notify();
    }
}

/// @brief 后端线程调用，把日志信息写入文件系统
void Lute::AsyncLogger::threadFunc() {
    assert(running_ == true);
    latch_.countDown();

    // LogFile output(basename_, rollSize_, false);
    LogFile output(basename_, rollSize_, false, flushInterval_);

    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();

    /// 申请一块大小为16的缓冲集，一般只会用到2块，除非前端写入速度太快
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);

    // currentBuffer_->length() 确保当前缓冲区数据写入完毕
    while (running_ || currentBuffer_->length() > 0) {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        /// Swap out what need to be written, keep CS short
        {
            MutexLockGuard lock(mutex_);
            if (buffers_.empty())  // unusual usage!
                cond_.waitForSeconds(flushInterval_);

            /// 采用move 提高效率
            buffers_.push_back(std::move(currentBuffer_));
            currentBuffer_ = std::move(newBuffer1);
            /// 内部指针交换 而非复制
            /// 最核心操作，前后端缓冲交换
            buffersToWrite.swap(buffers_);
            if (!nextBuffer_) nextBuffer_ = std::move(newBuffer2);
        }

        assert(!buffersToWrite.empty());

        /// 待写入缓冲集长度不对 输出错误
        /// 将错误数据写入文件，并裁剪待写入缓冲集
        if (buffersToWrite.size() > 25) {
            char buf[256];
            snprintf(buf, sizeof buf,
                     "Dropped log messages at %s, %zd larger buffers\n",
                     Timestamp::now().toFormattedString().c_str(),
                     buffersToWrite.size() - 2);
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));
            buffersToWrite.erase(buffersToWrite.begin() + 2,
                                 buffersToWrite.end());
        }

        /// 迭代待写入缓冲集，将缓冲日志写入文件系统
        for (const auto& buffer : buffersToWrite) {
            // FIXME: use unbuffered stdio FILE ? or use ::writev ?
            output.append(buffer->data(), buffer->length());
        }

        if (buffersToWrite.size() > 2) {
            // drop non-bzero-ed buffers, avoid trashing
            buffersToWrite.resize(2);
        }

        if (!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }

    output.flush();
}
