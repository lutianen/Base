#include <LuteBase.h>
#include <unistd.h>

#include <iostream>
#include <vector>

using std::vector;
size_t g_threadNum = 16;
size_t g_logNum = 1024 * 1024;

vector<Lute::Thread*> threads;

int main(int argc, char** argv) {
    std::cout << argc << ", " << argv[0] << std::endl;

    initLogger(Lute::Logger::LogLevel::TRACE);

    // LOG_TRACE << "This is log TRACE TEST";
    // LOG_DEBUG << "This is log DEBUG TEST";
    // LOG_INFO << "This is log INFO TEST";
    // LOG_WARN << "This is log WARN TEST";
    // LOG_ERROR << "This is log ERROR TEST";
    // LOG_SYSERR << "This is log SYSERR TEST";

    LOG_INFO << "CurrentThread: " << Lute::CurrentThread::tid();

    for (size_t i = 0; i < g_threadNum; ++i) {
        threads.push_back(new Lute::Thread([]() {
            for (size_t k = 0; k < g_logNum; ++k)
                LOG_INFO << "Hello 0123456789"
                            " abcdefghijklmnopqrstuvwxyz "
                            "Hello 0123456789"
                            " abcdefghijklmnopqrstuvwxyz "
                            "Hello 0123456789"
                            " abcdefghijklmnopqrstuvwxyz "
                            "Hello 0123456789"
                            " abcdefghijklmnopqrstuvwxyz "
                            "Hello 0123456789"
                            "  256Byte";
        }));
    }
    PING(a);
    for (const auto& thread : threads) thread->start();
    for (const auto& thread : threads) thread->join();
    PONG(a);
}