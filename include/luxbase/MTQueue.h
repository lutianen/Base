/**
 * @brief A thread-safe queue
 * @usage
    lux::base::MTQueue<int> que;
    std::thread tproducer([]() {
        for (int i = 0; i < 100; ++i) {
            que.push(i);
        }
    });
    std::thread tconsumer([]() {
        for (int i = 0; i < 100; ++i) {
            que.pop();
        }
    });

    tproducer.join();
    tconsumer.join();
 */

#include <condition_variable>
#include <mutex>
#include <vector>

namespace lux {
namespace base {

    /**
     * @brief A thread-safe queue
     * 
     * @tparam T The type of the elements
     */
    template <typename T>
    class MTQueue {
        std::condition_variable cv_;
        std::mutex mtx_;
        std::vector<T> queue_;

    public:
        T pop() {
#if __plusplus >= 201703L
            std::unique_lock lock(mtx_);
#else
            std::unique_lock<std::mutex> lock(mtx_);
#endif
            cv_.wait(lock, [this] { return !queue_.empty(); });
            T ret = std::move(queue_.back());
            queue_.pop_back();
            return ret;
        }

#if __cplusplus >= 201703L
        auto popHead() {
            std::unique_lock lock(mtx_);
#else
        auto popHead() -> std::pair<T, std::unique_lock<std::mutex> > {
            std::unique_lock<std::mutex> lock(mtx_);
#endif
            cv_.wait(lock, [this] { return !queue_.empty(); });
            T ret = std::move(queue_.back());
            queue_.pop_back();
#if __cplusplus >= 201703L
            return std::pair(std::move(ret), std::move(lock));
#else
            return std::pair<decltype(ret), decltype(lock)>(std::move(ret),
                                                            std::move(lock));
#endif
        }

        void push(T val) {
#if __cplusplus >= 201703L
            std::unique_lock lock(mtx_);
#else
            std::unique_lock<std::mutex> lock(mtx_);
#endif
            queue_.push_back(std::move(val));
            cv_.notify_one();
        }

        void pushMany(std::initializer_list<T> vals) {
#if __cplusplus >= 201703L
            std::unique_lock lock(mtx_);
            std::copy(std::move_iterator(vals.begin()),
                      std::move_iterator(vals.end()),
                      std::back_inserter(queue_));
#else
            std::unique_lock<std::mutex> lock(mtx_);
            std::copy(std::move_iterator<decltype(vals.begin())>(vals.begin()),
                      std::move_iterator<decltype(vals.end())>(vals.end()),
                      std::back_inserter(queue_));
#endif
            cv_.notify_all();
        }
    };

}  // namespace base
}  // namespace lux
