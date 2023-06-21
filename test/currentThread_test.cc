#include <luxbase/currentThread.h>
#include <luxbase/utils.h>

#include <iostream>

int main() {
    std::cout << lux::base::CurrentThread::tid() << std::endl;
    std::cout << lux::base::CurrentThread::name() << std::endl;
    std::cout << lux::base::CurrentThread::tidStringLength() << std::endl;
    std::cout << lux::base::CurrentThread::tidString() << std::endl;
    std::cout << lux::base::CurrentThread::isMainThread() << std::endl;
    std::cout << lux::base::CurrentThread::stackTrace(false) << std::endl;
    std::cout << " --- " << std::endl;
    std::cout << lux::base::CurrentThread::stackTrace(true) << std::endl;
    lux::base::CurrentThread::cacheTid();

    PING(sleepUsec);
    lux::base::CurrentThread::sleepUsec(1000);
    PONG(sleepUsec);

    return 0;
}