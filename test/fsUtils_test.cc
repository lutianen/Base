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
    return 0;
}