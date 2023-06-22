#include <dirent.h>  // opendir
#include <luxbase/fsUtils.h>
#include <signal.h>     // kill
#include <sys/types.h>  // DIR
#include <unistd.h>     // access

#include <cstring>  // strcmp
#include <string>   // string

using namespace lux;
using namespace lux::base;

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
                files.push_back(path + "/" + filename);
            } else {
                if (filename.size() < subfix.size()) {
                    continue;
                }
                if (filename.substr(filename.length() - subfix.size()) ==
                    subfix) {
                    files.push_back(path + "/" + filename);
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
        if (ptr != nullptr)
            break;
        else if (mkdir(path) != 0)
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
