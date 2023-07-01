#include <Base/fsUtils.h>
#include <Base/utils.h>

#include <memory>  // unique_ptr

int main() {
    std::vector<std::string> files;
    Lute::FSUtil::listAllFile(files, "/home/lux/Base", "");
    // for (auto& file : files) {
    //     std::cout << file << std::endl;
    // }
    std::string path(
        "/home/lux/Base/test_mkdir_func/"
        "test_mkdir_func1/test_mkdir_func2");
    PING(mkdir);
    std::cout << Lute::FSUtil::mkdir(path) << std::endl;
    PONG(mkdir);
    std::cout << Lute::FSUtil::rm("/home/lux/Base/test_mkdir_func")
              << std::endl;

    // PING(mkdir2);
    // std::cout << Lute::FSUtil::mkdir2(path) << std::endl;
    // PONG(mkdir2);
    std::cout << Lute::FSUtil::rm("/home/lux/Base/test_mkdir_func")
              << std::endl;

    std::cout << Lute::FSUtil::mv("/home/lux/Base/test_mv_file2.txt",
                                  "/home/lux/Base/test_mv_file.txt")
              << std::endl;
    std::cout << Lute::FSUtil::mv("/home/lux/Base/test_mv_file.txt",
                                  "/home/lux/Base/test_mv_file2.txt")
              << std::endl;

    std::string abspath;
    std::cout << Lute::FSUtil::realpath("./utils", abspath) << std::endl;
    std::cout << abspath << std::endl;

    std::cout << Lute::FSUtil::symlink("/home/lux/Base/test_mv_file2.txt",
                                       "/home/lux/Base/test_mv_file3.txt")
              << std::endl;

    std::cout << Lute::FSUtil::dirname(abspath) << std::endl;
    std::cout << Lute::FSUtil::basename(abspath) << std::endl;

    std::ifstream ifs;
    std::cout << Lute::FSUtil::openForRead(ifs,
                                           "/home/lux/Base/test_mv_file3.txt",
                                           std::ios_base::binary)
              << std::endl;

    size_t size = Lute::FSUtil::fileSize("/home/lux/Base/test_mv_file3.txt");

    std::unique_ptr<char[]> buf(new char[size + 1]);
    ifs.getline(buf.get(), size);
    std::cout << buf.get() << std::endl;

    std::ofstream ofs;
    std::cout << Lute::FSUtil::openForWrite(
                     ofs, "/home/lux/Base/test_mv_file3.txt",
                     std::ios_base::binary | std::ios_base::app)
              << std::endl;

    ofs << RED "Hello World" CLR << std::endl;

    {
        std::string result;
        int64_t size = 0;
        int err = Lute::readFile("/proc/self", 1024, &result, &size);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
        if (err != 0) std::cout << strerror(err) << std::endl;

        err = Lute::readFile("/proc/self", 1024, &result, NULL);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

        err = Lute::readFile("/proc/self/cmdline", 1024, &result, &size);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

        err = Lute::readFile("/dev/null", 1024, &result, &size);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

        err = Lute::readFile("/dev/zero", 1024, &result, &size);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

        err = Lute::readFile("/notexist", 1024, &result, &size);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

        err = Lute::readFile("/dev/zero", 102400, &result, &size);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

        err = Lute::readFile("/dev/zero", 102400, &result, NULL);
        printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    }

    {
        Lute::ReadSmallFile file("/tmp/test_append_file");
        auto size = new int(0);
        file.readToBuffer(size);
        std::cout << file.buffer() << std::endl;
        delete size;
    }

    {
        Lute::AppendFile file("/tmp/test_append_file");
        std::string str = "Hello World\n";
        file.append(str.c_str(), str.size());
        file.flush();
        std::cout << file.writtenBytes() << std::endl;
    }

    {
        assert(Lute::FSUtil::touch("/home/lux/test_touch/file1") != false);
        assert(Lute::FSUtil::touch("/home/lux/test_touch/file2") != false);
        assert(Lute::FSUtil::touch("/home/lux/test_touch/file3") != false);
    }

    return 0;
}