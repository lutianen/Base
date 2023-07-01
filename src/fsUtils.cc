#include <Base/fsUtils.h>
#include <dirent.h>  // opendir
#include <fcntl.h>   // open
#include <unistd.h>  // access

#include <cassert>  // assert
#include <csignal>  // kill
#include <cstring>  // strcmp
#include <memory>
#include <string>  // string

using namespace Lute;

void FSUtil::listAllFile(std::vector<std::string>& files,
                         const std::string& path, const std::string& subfix) {
    if (access(path.c_str(), 0) != 0) return;

    DIR* dir = opendir(path.c_str());
    if (dir == nullptr) return;

    struct dirent* dp = nullptr;
    while (nullptr != (dp = readdir(dir))) {
        // 目录文件
        if (dp->d_type == DT_DIR) {
            if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
                continue;
            }
            // Recursion
            listAllFile(files, path + "/" + dp->d_name, subfix);
        } else if (dp->d_type == DT_REG) /* 普通文件 */ {
            std::string filename(dp->d_name);
            if (subfix.empty()) {
                files.emplace_back(path + "/" + filename);
            } else {
                if (filename.size() < subfix.size()) {
                    continue;
                }
                if (filename.substr(filename.length() - subfix.size()) ==
                    subfix) {
                    files.emplace_back(path + "/" + filename);
                }
            }
        }
    }
    closedir(dir);
}

int FSUtil::lstat(const char* file, struct stat* st) {
    struct stat lst {};
    int rc = ::lstat(file, &lst);
    if (nullptr != st) *st = lst;
    return rc;
}

int FSUtil::mkdir(const char* dirname) {
    if (access(dirname, F_OK) == 0) return 0;
    return ::mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

bool FSUtil::mkdir(const std::string& dirname) {
    if (lstat(dirname.c_str()) == 0) return true;

    // malloc
    char* path = strdup(dirname.c_str());
    char* ptr = std::strchr(path + 1, '/');
    do {
        for (; ptr != nullptr; *ptr = '/', ptr = std::strchr(ptr + 1, '/')) {
            *ptr = '\0';
            if (mkdir(path) != 0) break;
        }
        if (ptr != nullptr) {
            break;
        } else if (mkdir(path) != 0)
            break;
        free(path);
        return true;
    } while (false);

    free(path);
    return false;
}

// #include <memory>  // unique_ptr
// bool FSUtil::mkdir2(const std::string& dirname) {
//     if (lstat(dirname.c_str()) == 0) return true;

//     // malloc
//     std::unique_ptr<char[]> path(new char[dirname.size() + 1]);
//     std::strcpy(path.get(), dirname.c_str());
//     char* ptr = std::strchr(path.get() + 1, '/');
//     do {
//         for (; ptr != nullptr; *ptr = '/', ptr = std::strchr(ptr + 1, '/')) {
//             *ptr = '\0';
//             if (mkdir(path.get()) != 0) break;
//         }
//         if (ptr != nullptr)
//             break;
//         else if (mkdir(path.get()) != 0)
//             break;
//         return true;
//     } while (false);

//     return false;
// }

bool FSUtil::touch(const std::string& filename) {
    auto file = basename(filename);
    auto dir = dirname(filename);
    if (!dir.empty() && !mkdir(dir)) return false;

    /// O_CREAT: 若文件不存在，则创建它
    int fd = ::open(filename.c_str(), O_CREAT, 0644);
    if (fd == -1) return false;
    close(fd);

    return true;
}

bool FSUtil::isRunningPidfile(const std::string& pidfile) {
    if (lstat(pidfile.c_str()) != 0) return false;

    std::ifstream ifs(pidfile);
    std::string line;
    if (!ifs || !std::getline(ifs, line)) return false;
    if (line.empty()) return false;

    pid_t pid = std::stoi(line);
    if (pid <= 1) return false;

    /* kill(pid, 0)
     * 用于向进程或进程组发送一个空信号，起作用是检测进程或进程组的存在性和权限
     * 如果进程或进程组存在且拥有足够的权限，返回0，否则返回-1
     */
    if (::kill(pid, 0) != 0) return false;
    return true;
}

bool FSUtil::unlink(const std::string& filename, bool exist) {
    if (!exist && lstat(filename.c_str()) != 0) return true;
    return ::unlink(filename.c_str()) == 0;
}

bool FSUtil::rm(const std::string& path) {
    struct stat st {};
    if (lstat(path.c_str(), &st) != 0) return true;
    // 非目录，则直接删除 - unlink
    if (!(st.st_mode & S_IFDIR)) return unlink(path);

    // 目录，则递归删除 - rmdir
    DIR* dir = opendir(path.c_str());
    if (dir == nullptr) return false;

    bool rc = true;
    struct dirent* dp = nullptr;
    while ((dp = readdir(dir))) {
        if (!std::strcmp(dp->d_name, ".") || !std::strcmp(dp->d_name, ".."))
            continue;

        std::string filename = path + "/" + dp->d_name;
        rc = rm(filename);
    }
    closedir(dir);
    if (::rmdir(path.c_str()) != 0) rc = false;
    return rc;
}

bool FSUtil::mv(const std::string& from, const std::string& to) {
    if (lstat(from.c_str()) != 0) return false;
    if (rm(to) != 0) return false;
    return ::rename(from.c_str(), to.c_str()) == 0;
}

bool FSUtil::realpath(const std::string& path, std::string& rpath) {
    if (lstat(path.c_str()) != 0) return false;
    char* ptr = ::realpath(path.c_str(), nullptr);
    if (ptr == nullptr) return false;
    std::string(ptr).swap(rpath);
    free(ptr);
    return true;
}

bool FSUtil::symlink(const std::string& from, const std::string& to) {
    if (lstat(from.c_str()) != 0) return false;
    if (rm(to) == 0) return false;
    return ::symlink(from.c_str(), to.c_str()) == 0;
}

std::string FSUtil::dirname(const std::string& filename) {
    if (filename.empty()) return ".";
    auto pos = filename.rfind('/');
    if (pos == 0) return "/";
    if (pos == std::string::npos) return ".";
    return filename.substr(0, pos);
}

std::string FSUtil::basename(const std::string& filename) {
    if (filename.empty()) return filename;
    auto pos = filename.rfind('/');
    if (pos == std::string::npos) return filename;
    return filename.substr(pos + 1);
}

bool FSUtil::openForRead(std::ifstream& ifs, const std::string& filename,
                         std::ios_base::openmode mode) {
    ifs.open(filename, mode);
    return ifs.is_open();
}

bool FSUtil::openForWrite(std::ofstream& ofs, const std::string& filename,
                          std::ios_base::openmode mode) {
    ofs.open(filename, mode);
    if (!ofs.is_open()) {
        std::string dir = dirname(filename);
        mkdir(dir);
        ofs.open(filename, mode);
    }
    return ofs.is_open();
}

size_t FSUtil::fileSize(const std::string& filename) {
    if (lstat(filename.c_str()) != 0) return 0;
    std::ifstream ifs;
    if (openForRead(ifs, filename, std::ios_base::binary)) {
        ifs.seekg(0, std::ios_base::end);
        return ifs.tellg();
    } else
        return 0;
}

ReadSmallFile::ReadSmallFile(const std::string& filename)
    : fd_(::open(filename.c_str(), O_RDONLY | O_CLOEXEC)), err_(0) {
    buf_[0] = '\0';
    if (fd_ < 0) err_ = errno;
}

///
/// @brief Close file descriptor
///
ReadSmallFile::~ReadSmallFile() {
    if (fd_ >= 0) {
        int rc;
        do {
            rc = ::close(fd_);
        } while (rc == -1 && errno == EINTR);
    }
}

// return errno
template <typename String>
int ReadSmallFile::readToString(int maxSize, String* content, int64_t* fileSize,
                                int64_t* modifyTime, int64_t* createTime) {
    static_assert(sizeof(off_t) == 8, "_FILE_OFFSET_BITS = 64");
    assert(content != NULL);
    int err = err_;
    if (fd_ >= 0) {
        content->clear();

        if (fileSize) {
            struct stat statbuf {};
            if (::fstat(fd_, &statbuf) == 0) {
                if (S_ISREG(statbuf.st_mode)) /* Regular file */ {
                    *fileSize = statbuf.st_size;
                    content->reserve(static_cast<int>(
                        std::min(static_cast<int64_t>(maxSize), *fileSize)));
                } else if (S_ISDIR(statbuf.st_mode)) /* Directory */
                    err = EISDIR;

                if (modifyTime) *modifyTime = statbuf.st_mtime;
                if (createTime) *createTime = statbuf.st_ctime;
            } else
                err = errno;
        }

        while (content->size() < static_cast<size_t>(maxSize)) {
            size_t toRead = std::min(maxSize - content->size(), sizeof(buf_));
            ssize_t n = ::read(fd_, buf_, toRead);
            if (n > 0) {
                content->append(buf_, n);
            } else {
                if (n < 0) err = errno;
                break;
            }
        }
    }
    return err;
}

int ReadSmallFile::readToBuffer(int* size) {
    int err = err_;
    if (fd_ >= 0) {
        ssize_t n = ::pread(fd_, buf_, sizeof(buf_) - 1, 0);
        if (n >= 0) {
            if (size) *size = static_cast<int>(n);
            buf_[n] = '\0';
        } else {
            err = errno;
        }
    }
    return err;
}

AppendFile::AppendFile(const std::string& filename)
    : fp_(::fopen(filename.c_str(), "ae")), writtenBytes_(0) {
    assert(fp_);
    ::setbuffer(fp_, buffer_, sizeof(buffer_));
}

AppendFile::~AppendFile() { ::fclose(fp_); }

size_t AppendFile::write(const char* logline, size_t len) {
    /// 1 - 数据类型大小
    return ::fwrite_unlocked(logline, 1, len, fp_);
}

void AppendFile::append(const char* logline, const size_t len) {
    size_t written = 0;
    while (written != len) {
        size_t remain = len - written;
        size_t n = write(logline + written, remain);
        if (n != remain) {
            int err = ::ferror(fp_);
            if (err) {
                // NOTE 512 is _POSIX_ERROR_MAX

#if __cplusplus >= 201703L
                auto buf = std::make_unique<char[]>(512);
#else
                std::unique_ptr<char[]> buf(new char[512]);
#endif

                ::fprintf(stderr, "AppendFile::append() failed %s\n",
                          ::strerror_r(err, buf.get(), 512));
                break;
            }
        }
        written += n;
    }
    writtenBytes_ += written;
}

void AppendFile::flush() { ::fflush(fp_); }

template int Lute::readFile(const std::string& filename, int maxSize,
                            std::string* content, int64_t*, int64_t*, int64_t*);
