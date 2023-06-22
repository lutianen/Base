/**
 * @brief File system utilities
 * 
 */

#pragma once

#include <sys/stat.h>  // stat

#include <fstream>
#include <string>
#include <vector>

namespace lux {
namespace base {
    class FSUtil {
    public:
        /// @brief List all files in path with subfix
        static void listAllFile(std::vector<std::string>& files,
                                const std::string& path,
                                const std::string& subfix);

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
        static bool openForWrite(std::ofstream& ofs,
                                 const std::string& filename,
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

}  // namespace base
}  // namespace lux
