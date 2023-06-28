/**
 * @brief File system utilities
 *
 */

#pragma once

#include <sys/stat.h>  // stat

#include <cstring>  // strerror_r
#include <fstream>
#include <string>
#include <vector>

namespace Lute {
class FSUtil {
public:
    /// @brief List all files in path with subfix
    static void listAllFile(std::vector<std::string>& files,
                            const std::string& path, const std::string& subfix);

    /// @brief Make diectories recursively
    static bool mkdir(const std::string& dirname);
    // static bool mkdir2(const std::string& dirname);

    /// @brief Check file is exist
    static bool isRunningPidfile(const std::string& pidfile);

    /// @brief Unlink a file
    static bool unlink(const std::string& filename, bool exist = false);

    /// @brief Remove path recursively
    static bool rm(const std::string& path);

    /// @brief Move file or directory from `from` to `to`
    /// @note If `from` isn't exist, return false;
    static bool mv(const std::string& from, const std::string& to);

    /// @brief Transfrom relative path to absolute path
    static bool realpath(const std::string& path, std::string& rpath);

    /// @brief Create a symlink
    /// @note If `from` isn't exist, return false;
    static bool symlink(const std::string& frm, const std::string& to);

    /// @brief Get directory of filename
    static std::string dirname(const std::string& filename);

    /// @brief Get basename of filename
    static std::string basename(const std::string& filename);

    /// @brief Open file for read
    static bool openForRead(std::ifstream& ifs, const std::string& filename,
                            std::ios_base::openmode mode);

    /// @brief Open file for write
    static bool openForWrite(std::ofstream& ofs, const std::string& filename,
                             std::ios_base::openmode mode);

    /// @brief Get file size
    static size_t fileSize(const std::string& filename);

private:
    /// @brief Get file attributes about FILE and put them in ST.
    static inline int lstat(const char* file, struct stat* st = nullptr);

    /// @brief Create a directory
    /// @attr S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
    static inline int mkdir(const char* dirname);
};

/// @brief read small file < 64KB
class ReadSmallFile {
public:
    static const int kBufferSize = 64 * 1024;

    /// non-copyable
    ReadSmallFile(const ReadSmallFile&) = delete;
    ReadSmallFile(ReadSmallFile&) = delete;

    // ReadSmallFile(StringArg filename);
    explicit ReadSmallFile(const std::string& filename);
    ~ReadSmallFile();

    ///
    /// @brief Read the file content, returns errno if error happens.
    ///
    /// @return int errno
    ///
    template <typename String>
    int readToString(int maxSize, String* content, int64_t* fileSize,
                     int64_t* modifyTime, int64_t* createTime);

    ///
    /// @brief Read file to buf_ and set fileSize to size
    ///
    /// @param size read size
    /// @return int errno
    ///
    int readToBuffer(int* size);

    const char* buffer() const { return buf_; }

private:
    int fd_;
    int err_;
    char buf_[kBufferSize]{};
};

/// @brief read the file content, returns errno if error happens.
template <typename String>
// int readFile(StringArg filename, int maxSize, String* content,
int readFile(const std::string& filename, int maxSize, String* content,
             int64_t* fileSize = nullptr, int64_t* modifyTime = nullptr,
             int64_t* createTime = nullptr) {
    ReadSmallFile file(filename);
    return file.readToString(maxSize, content, fileSize, modifyTime,
                             createTime);
}

///
/// @brief Append content to file
/// @note Not thread safe
///
class AppendFile {
public:
    AppendFile(const AppendFile&) = delete;
    AppendFile(AppendFile&) = delete;

    // explicit AppendFile(StringArg filename);
    ///
    /// @brief fopen 的模式为 ae; 'e' for O_CLOEXEC - 表示以针对 O_CLOEXEC
    ///             标志的扩展模式打开文件，表示文件在执行 exec
    ///             系统调用时自动关闭
    ///
    explicit AppendFile(const std::string& filename);
    ~AppendFile();

    ///
    /// @brief Write logline to fp_
    ///
    /// @param logline - 日志内容
    /// @param len - 日志内容长度
    ///
    void append(const char* logline, size_t len);

    void flush();

    off_t writtenBytes() const { return writtenBytes_; }

private:
    ///
    /// @brief Write logline to fp_
    ///
    /// @param logline - 日志内容
    /// @param len - 日志内容长度
    ///
    size_t write(const char* logline, size_t len);

    FILE* fp_;                // FILE*
    char buffer_[64 * 1024];  // 64KB
    off_t writtenBytes_;      // 已经写入的字节数
};

}  // namespace Lute
