#include <luxbase/condition_variable.h>

#include <iostream>

bool lux::base::Condition::waitForSeconds(double seconds) {
    struct timespec abstime;

    // FIXME: use CLOCK_MONOTONIC or CLOCK_MONOTONIC_RAW to prevent time rewind.
    /**
     * 在 Linux 系统中，有多种时钟可以用来获取时间信息:
     *      CLOCK_REALTIME, CLOCK_MONOTONIC, CLOCK_MONOTIC_RAW 等
     * CLOCK_REALTIME: 系统实时时间，随系统实时时间改变而改变，
        即从UTC1970-1-1 0:0:0开始计时;
        如果系统时钟被用户或管理员手动修改，可能发生时间倒流;
     * CLOCK_MONOTONIC / CLOCK_MONOTONIC_RAW:
        从系统启动这一刻起开始计时，不受系统修改的应影响;
     * 区别:
        CLOCK_MONOTONIC 时钟可能会受到时间调整的影响（例如 NTP 校时），
        CLOCK_MONOTONIC_RAW 时钟则不受影响
     */
    ::clock_gettime(CLOCK_REALTIME, &abstime);

    timespec ts;
#ifdef CLOCK_MONOTONIC_RAW
    ::clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
#else
    ::clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

    std::cout << abstime.tv_sec << ", " << abstime.tv_nsec << ", " << ts.tv_sec
              << ", " << ts.tv_nsec << std::endl;

    const int64_t kNanoSecondsPerSecond = 1E9;
    int64_t nanoseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);

    abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) /
                                          kNanoSecondsPerSecond);
    abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) %
                                        kNanoSecondsPerSecond);

    MutexLock::UnassignGuard ug(mutex_);
    return ETIMEDOUT == ::pthread_cond_timedwait(
                            &pcond_, mutex_.getPthreadMutex(), &abstime);
}
