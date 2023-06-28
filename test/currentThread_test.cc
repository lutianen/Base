#include <Base/currentThread.h>
#include <Base/utils.h>

#include <iostream>

int main() {
    std::cout << Lute::CurrentThread::tid() << std::endl;
    std::cout << Lute::CurrentThread::name() << std::endl;
    std::cout << Lute::CurrentThread::tidStringLength() << std::endl;
    std::cout << Lute::CurrentThread::tidString() << std::endl;
    std::cout << Lute::CurrentThread::isMainThread() << std::endl;
    std::cout << Lute::CurrentThread::stackTrace(false) << std::endl;
    std::cout << " --- " << std::endl;
    std::cout << Lute::CurrentThread::stackTrace(true) << std::endl;
    Lute::CurrentThread::cacheTid();

    PING(sleepUsec);
    Lute::CurrentThread::sleepUsec(1000);
    PONG(sleepUsec);

    return 0;
}