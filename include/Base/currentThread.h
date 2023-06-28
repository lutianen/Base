/**
 * @brief The useful information of Current thread.
 * t_cacheTid - current thread ID
 * t_tidString
 * t_tidStringLength
 * t_threadName - current thread name
 */

#pragma once

#include <string>

namespace Lute {
namespace CurrentThread {
    // internal
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char* t_threadName;

    // impl in Thread.cc
    void cacheTid();

    inline int tid() {
        // t_cacheTid == 0 is likely to be false
        if (__builtin_expect(t_cachedTid == 0, 0)) cacheTid();
        return t_cachedTid;
    }

    // for logging
    inline const char* tidString() { return t_tidString; }
    // for logging
    inline int tidStringLength() { return t_tidStringLength; }

    inline const char* name() { return t_threadName; }

    // impl in Thread.cc
    bool isMainThread();

    /// @brief sleep for n useconds (1/1000000 second)
    /// for testing
    void sleepUsec(int64_t usec);

    /**
     * @brief stackTrace
     * @param demangle - 是否进行符号解析
     * @return std::string
     */
    std::string stackTrace(bool demangle = false);
}  // namespace CurrentThread
}  // namespace Lute