#include <luxbase/condition_variable.h>
#include <luxbase/mutex.h>
#include <luxbase/thread.h>
#include <luxbase/utils.h>

#include <vector>

using namespace std;
using namespace lux::base;

MutexLock g_mutex;
vector<int> g_vec;
const int kCount = 10 * 1000 * 1000;

void threadFunc() {
    for (size_t i = 0; i < kCount; ++i) {
        MutexLockGuard lock(g_mutex);
        g_vec.push_back(i);
    }
}

int foo() __attribute__((noinline));

int g_count = 0;

int foo() {
    MutexLockGuard lock(g_mutex);
    if (!g_mutex.isLockedByThisThread()) {
        printf("FAIL\n");
        return -1;
    }

    ++g_count;
    return 0;
}

int main() {
    printf("sizeof pthread_mutex_t: %zd\n", sizeof(pthread_mutex_t));
    printf("sizeof Mutex: %zd\n", sizeof(MutexLock));
    printf("sizeof pthread_cond_t: %zd\n", sizeof(pthread_cond_t));
    printf("sizeof Condition: %zd\n", sizeof(Condition));
    MCHECK(foo());
    if (g_count != 1) {
        printf("MCHECK calls twice.\n");
        abort();
    }

    PING(sigleThreadWithoutLock);
    const int kMaxThreads = 16;
    g_vec.reserve(kMaxThreads * kCount);
    for (size_t i = 0; i < kCount; ++i) {
        g_vec.push_back(i);
    }
    PONG(sigleThreadWithoutLock);
    

    PING(sigleThreadWithLock);
    threadFunc();
    PONG(sigleThreadWithLock);

    for (size_t nthreads = 0; nthreads < kMaxThreads; ++nthreads) {
        std::vector<std::unique_ptr<Thread>> threads;
        g_vec.clear();

        PING(nthreadsThreads);
        for (size_t i = 0; i < nthreads; ++i) {
            threads.emplace_back(new Thread(&threadFunc));
            threads.back()->start();
        }
        for (size_t i = 0; i < nthreads; ++i) {
            threads[i]->join();
        }
        PONG(nthreadsThreads);
    }

    return 0;
}