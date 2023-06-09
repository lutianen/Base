/**
 * @brief 核心函数
 *      start 调用线程函数
 *      join 等待主线程回收
 */

#pragma once

#include <luxbase/countDownLatch.h>  // CountDownLatch
#include <pthread.h>                 // pthread_t

#include <atomic>
#include <functional>  // function
#include <memory>

namespace lux {
namespace base {

    /**
     * pthread 核心函数：线程的创建和等待结束
     * pthread_create
     * pthread_join
     */
    class Thread {
    public:
        using ThreadFunc = std::function<void()>;

    private:
        bool started_;
        bool joined_;

        /* pthread_t 不适合做程序中对线程的标识符, 只在进程内具有唯一性 */
        pthread_t pthreadId_;
        // 在操纵系统内具有全局唯一性，采用递增轮回法进行分配
        pid_t tid_;

        // 线程函数
        ThreadFunc func_;
        std::string name_;
        lux::base::CountDownLatch latch_;

        // 用于线程池
        static std::atomic_int32_t numCreated_;

        // noncopyable
        Thread(const Thread&) = delete;
        Thread& operator=(Thread&) = delete;

        void setDefaultName();

    public:
        explicit Thread(ThreadFunc, std::string name = std::string());
        // FIXME: make it movable in C++11
        ~Thread();

        void start();
        int join();  // return pthread_join()

        bool started() const { return started_; }

        // pthread_t
        // pthreadId() const { return pthreadId_; }

        pid_t tid() const { return tid_; }

        const std::string& name() const { return name_; }

        static int numCreated() { return numCreated_.load(); }
    };
}  // namespace base
}  // namespace Lux