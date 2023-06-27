#include <luxbase/fsUtils.h>
#include <luxbase/utils.h>

#include <memory>  // unique_ptr

int main() {
    std::vector<std::string> files;
    lux::base::FSUtil::listAllFile(files, "/home/lux/LuxBase", "");
    // for (auto& file : files) {
    //     std::cout << file << std::endl;
    // }
    std::string path(
        "/home/lux/LuxBase/test_mkdir_func/"
        "test_mkdir_func1/test_mkdir_func2");
    PING(mkdir);
    std::cout << lux::base::FSUtil::mkdir(path) << std::endl;
    PONG(mkdir);
    std::cout << lux::base::FSUtil::rm("/home/lux/LuxBase/test_mkdir_func")
              << std::endl;

    // PING(mkdir2);
    // std::cout << lux::base::FSUtil::mkdir2(path) << std::endl;
    // PONG(mkdir2);
    std::cout << lux::base::FSUtil::rm("/home/lux/LuxBase/test_mkdir_func")
              << std::endl;

    std::cout << lux::base::FSUtil::mv("/home/lux/LuxBase/test_mv_file2.txt",
                                       "/home/lux/LuxBase/test_mv_file.txt")
              << std::endl;
    std::cout << lux::base::FSUtil::mv("/home/lux/LuxBase/test_mv_file.txt",
                                       "/home/lux/LuxBase/test_mv_file2.txt")
              << std::endl;

    std::string abspath;
    std::cout << lux::base::FSUtil::realpath("./utils", abspath) << std::endl;
    std::cout << abspath << std::endl;

    std::cout << lux::base::FSUtil::symlink(
                     "/home/lux/LuxBase/test_mv_file2.txt",
                     "/home/lux/LuxBase/test_mv_file3.txt")
              << std::endl;

    std::cout << lux::base::FSUtil::dirname(abspath) << std::endl;
    std::cout << lux::base::FSUtil::basename(abspath) << std::endl;

    std::ifstream ifs;
    std::cout << lux::base::FSUtil::openForRead(
                     ifs, "/home/lux/LuxBase/test_mv_file3.txt",
                     std::ios_base::binary)
              << std::endl;

    size_t size =
        lux::base::FSUtil::fileSize("/home/lux/LuxBase/test_mv_file3.txt");

    std::unique_ptr<char[]> buf(new char[size + 1]);
    ifs.getline(buf.get(), size);
    std::cout << buf.get() << std::endl;

    std::ofstream ofs;
    std::cout << lux::base::FSUtil::openForWrite(
                     ofs, "/home/lux/LuxBase/test_mv_file3.txt",
                     std::ios_base::binary | std::ios_base::app)
              << std::endl;

    ofs << RED "Hello World" CLR << std::endl;

    {
        std::string result;
        int64_t size = 0;
        int err = lux::base::readFile("/proc/self", 1024, &result, &size);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
        if (err != 0) std::cout << strerror(err) << std::endl;

        err = lux::base::readFile("/proc/self", 1024, &result, NULL);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

        err = lux::base::readFile("/proc/self/cmdline", 1024, &result, &size);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

        err = lux::base::readFile("/dev/null", 1024, &result, &size);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

        err = lux::base::readFile("/dev/zero", 1024, &result, &size);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

        err = lux::base::readFile("/notexist", 1024, &result, &size);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

        err = lux::base::readFile("/dev/zero", 102400, &result, &size);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

        err = lux::base::readFile("/dev/zero", 102400, &result, NULL);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    }

    {
        lux::base::ReadSmallFile file("/tmp/test_append_file");
        auto size = new int(0);
        file.readToBuffer(size);
        std::cout << file.buffer() << std::endl;
        delete size;
    }

    {
        lux::base::AppendFile file("/tmp/test_append_file");
        std::string str = "Hello World\n";
        file.append(str.c_str(), str.size());
        file.flush();
        std::cout << file.writtenBytes() << std::endl;
    }

    return 0;
}