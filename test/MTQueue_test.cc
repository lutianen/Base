#include <luxbase/MTQueue.h>
#include <luxbase/pingpong.h>

#include <atomic>
#include <iostream>
#include <thread>

std::atomic_int count(0);
lux::base::MTQueue<int> que;

int main() {
    int c = 0;
    PING(testMTQ);
    for (int i = 0; i < 1e4; ++i) {
        std::thread tproducer([]() {
            for (int i = 0; i < 100; ++i) {
                que.push(i);
                count.fetch_add(1);
            }
        });
        std::thread tconsumer([]() {
            for (int i = 0; i < 100; ++i) {
                que.pop();
                count.fetch_sub(1);
            }
        });

        tproducer.join();
        tconsumer.join();

        if (count != 0) ++c;
    }
    PONG(testMTQ);

    std::cout << c << std::endl;
}