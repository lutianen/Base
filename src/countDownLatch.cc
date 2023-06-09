#include <luxbase/countDownLatch.h>

lux::base::CountDownLatch::CountDownLatch(int count)
    : mutex_(), condition_(mutex_), count_(count) {}

void lux::base::CountDownLatch::wait() {
    MutexLockGuard lock(mutex_);
    while (count_ > 0) {
        condition_.wait();
    }
}

void lux::base::CountDownLatch::countDown() {
    MutexLockGuard lock(mutex_);
    --count_;
    if (count_ == 0) {
        condition_.notifyAll();
    }
}

int lux::base::CountDownLatch::getCount() const {
    MutexLockGuard lock(mutex_);
    return count_;
}
