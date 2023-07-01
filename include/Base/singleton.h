///
/// @brief Singleton 模式
///

#pragma once

#include <memory>

namespace Lute {
namespace {
    template <typename T, typename X, int N>
    T& GetInstanceX() {
        static T v;
        return v;
    }

    template <typename T, typename X, int N>
    std::shared_ptr<T> GetInstancePtr() {
        static std::shared_ptr<T> v(new T);
        return v;
    }
}  // namespace

///
/// @brief 单例模式封装类
///
/// @tparam T 类型
/// @tparam X 为了创造多个实例对应的Tag
/// @tparam N 同一个Tag创造多个实例索引
///
template <class T, class X = void, int N = 0>
class Singleton {
public:
    ///
    /// @brief 返回单例裸指针
    ///
    static T* GetInstance() {
        static T v;
        return &v;
        // return &GetInstanceX<T, X, N>();
    }
};

///
/// @brief 单例模式智能指针封装类
/// @tparam T 类型
/// @tparam X 为了创造多个实例对应的Tag
/// @tparam N 同一个Tag创造多个实例索引
///
template <class T, class X = void, int N = 0>
class SingletonPtr {
public:
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> v(new T);
        return v;
        // return GetInstancePtr<T, X, N>();
    }

    template <typename... Arg>
    static std::shared_ptr<T> GetInstance(Arg... args) {
        static std::shared_ptr<T> v(new T(args...));
        return v;
        // return GetInstancePtr<T, X, N>();
    }
};

}  // namespace Lute