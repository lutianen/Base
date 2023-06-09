#include <luxbase/MTQueue.h>
#include <luxbase/pingpong.h>

#include <iostream>
#include <mutex>
#include <thread>

int count = 0;
lux::base::MTQueue<int> que;
std::mutex mtx;

int main() {
    int c = 0;
    PING(testMTQ);
    for (int i = 0; i < 1e4; ++i) {
        std::thread tproducer([]() {
            for (int i = 0; i < 100; ++i) {
                que.push(i);
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    count++;
                }
            }
        });
        std::thread tconsumer([]() {
            for (int i = 0; i < 100; ++i) {
                que.pop();
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    count--;
                }
            }
        });

        tproducer.join();
        tconsumer.join();

        if (count != 0) ++c;
    }
    PONG(testMTQ);

    std::cout << c << std::endl;
}