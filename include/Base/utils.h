/**
 * @brief Some utils
 *   - PING PONG
 *   - toUpper
 *   - toLower
 */

#pragma once

#include <Base/timestamp.h>  // Timestamp

#include <algorithm>  // toupper
#include <cassert>    // assert
#include <chrono>     // chrono
#include <cinttypes>  // PRId64
#include <cstring>
#include <iostream>  // cout endl
#include <random>    // mt19937
#include <string>    // string

/// 启用 C 标准库中一些格式化输出相关的宏定义和函数
/// PRId64, %zd, %zu, ...
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#if defined(__clang__)
/// 警告用于检测无意义的比较操作
#pragma clang diagnostic ignored "-Wtautological-compare"
#else
/// 警告用于检测在比较时使用了错误的类型
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

/// \033[ 表示控制码开始
#define ESC "\033["

/// \033[0m 关闭所有属性
#define CLR ESC "0m"

/// \033[01;32m 01表示加粗，31表示红色字体
/// NOTE Only support `RED "Hello Base" CLR` `REDs("Hello Base")` style
#define RED ESC "01;31m"
#define REDs(x) ESC "01;31m" x CLR
#define GREEN ESC "01;32m"
#define GREENs(x) ESC "01;32m" x CLR
#define YELLOW ESC "01;33m"
#define YELLOWs(x) ESC "01;33m" x CLR
#define BLUE ESC "01;34m"
#define BLUEs(x) ESC "01;34m" x CLR
#define PURPLE ESC "01;35m"
#define PURPLEs(x) ESC "01;35m" x CLR
#define DEEPGREEN ESC "01;36m"
#define DEEPGREENs(x) ESC "01;36m" x CLR
#define WHITE ESC "01;37m"
#define WHITEs(x) ESC "01;37m" x CLR

/// __attribute__((used))
const char digits[] __attribute__((used)) = "9876543210123456789";
static_assert(sizeof digits == 20, "wrong number of digits");
const char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof digitsHex == 17, "wrong number of digitsHex");

namespace Lute {

///
/// @brief Simple macro to measure time of execution of a piece of code
/// @usage
///     PING(CodeToBeMeasured)
///     CodeToBeMeasure
///     PONG(CodeToBeMeasured)
///
#define PING(x) auto bench_##x = std::chrono::steady_clock::now();
#define PONG(x)                                                             \
    std::cout << #x ": "                                                    \
              << std::chrono::duration_cast<std::chrono::duration<double>>( \
                     std::chrono::steady_clock::now() - bench_##x)          \
                     .count()                                               \
              << "s" << std::endl;

/// @brief Set N bytes of S to zero.
/// @param S void*
/// @param N size_t
inline void memZero(void* S, size_t N) { ::memset(S, 0, N); }

/// @brief Transform str to uppercase
std::string toUpper(const std::string& str);

/// @brief str to lowercase
std::string toLower(const std::string& str);

#define seed 42
static std::mt19937 __gen(seed);
#undef seed
///
/// @brief mt19937 + uniform_distribution (均匀分布)
/// @param T - char / short / int / long / uint32_t / float / double
/// @par seed is a micro [It can be modified.]
/// @usage randomUniform<int>() \n
///        randomUniform<int, 1>() \n
///        randomUniform<int, 1, 100>() \n
///        randomUniform<float, 1, 100>() ...
///
template <typename T = uint32_t, int minv = 1, int maxv = 10>
inline T randomUniform() {
#if __cplusplus >= 201703L
    if (std::is_integral_v<T>) {
#else
    if (std::is_integral<T>::value) {
#endif
        std::uniform_int_distribution<> dist(minv, maxv);
        return static_cast<T>(dist(__gen));
#if __cplusplus >= 201703L
    } else if (std::is_floating_point_v<T>) {
#else
    } else if (std::is_floating_point<T>::value) {
#endif
        std::uniform_real_distribution<> dist(minv, maxv);
        return static_cast<T>(dist(__gen));
    }
};

///
/// @brief Format a number with 5 characters, including SI units.
///   [0,     999]
///   [1.00k, 999k]
///   [1.00M, 999M]
///   [1.00G, 999G]
///   [1.00T, 999T]
///   [1.00P, 999P]
///   [1.00E, inf)
///
std::string formatSI(int64_t s);

///
/// @brief Format a number with 5 characters, including SI units.
/// [0, 1023]
/// [1.00Ki, 9.99Ki]
/// [10.0Ki, 99.9Ki]
/// [ 100Ki, 1023Ki]
/// [1.00Mi, 9.99Mi]
///
std::string formatIEC(int64_t s);

///
/// @brief Efficient Integer to String Conversions, by Matthew Wilson.
/// @param buf Dst buffer
/// @param value Src integer value
/// @return size_t Valid length of buf
///
template <typename T>
inline size_t integer2Str(char buf[], T value) {
    T i = value;
    char* p = buf;
    const char* zero = digits + 9;
    do {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);
    if (value < 0) *p++ = '-';
    *p = '\0';
    std::reverse(buf, p);
    return static_cast<size_t>(p - buf);
}

///
/// @brief Efficient Integer to Hex String Conversions, by Matthew Wilson.
///
/// @param buf Dst buffer
/// @param value Src integer value
/// @return size_t Valid length of buf
///
inline size_t integer2StrHex(char buf[], uintptr_t value) {
    uintptr_t i = value;
    char* p = buf;
    do {
        int lsd = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[lsd];
    } while (i != 0);
    *p = '\0';
    std::reverse(buf, p);
    return static_cast<size_t>(p - buf);
}

class ProcessInfo {
public:
    static pid_t pid();
    static std::string pidString();
    static uid_t uid();
    static std::string username();
    static uid_t euid();
    static Timestamp startTime();
    static int clockTicksPerSecond();
    static int pageSize();
    static bool isDebugBuild();  // constexpr

    static std::string hostname();
    static std::string procname();
    static std::string procname(const std::string& stat);

    /// read /proc/self/status
    static std::string procStatus();

    /// read /proc/self/stat
    static std::string procStat();

    /// read /proc/self/task/tid/stat
    static std::string threadStat();

    /// readlink /proc/self/exe
    static std::string exePath();

    static int openedFiles();
    static int maxOpenFiles();

    struct CpuTime {
        double userSeconds;
        double systemSeconds;

        CpuTime() : userSeconds(0.0), systemSeconds(0.0) {}

        double total() const { return userSeconds + systemSeconds; }
    };
    static CpuTime cpuTime();

    static int numThreads();
    static std::vector<pid_t> threads();
};

}  // namespace Lute
