/**
 * @brief Some utils
 *   - PING PONG
 *   - toUpper
 *   - toLower
 */

#pragma once

#include <algorithm>  // toupper
#include <chrono>     // chrono
#include <iostream>   // cout endl
#include <random>     // mt19937
#include <string>     // string

/// \033[ 表示控制码开始
#define ESC "\033["

/// \033[0m 关闭所有属性
#define CLR ESC "0m"

/// \033[01;32m 01表示加粗，31表示红色字体
/// NOTE Only support `RED "Hello LuxBase" CLR` `REDs("Hello LuxBase")` style
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

namespace lux {
namespace base {

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

    /// @brief Transform str to uppercase
    std::string toUpper(const std::string& str);

    /// @brief str to lowercase
    std::string toLower(const std::string& str);

#define seed 42
    static std::mt19937 __gen(seed);
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
        if (std::is_integral_v<T>) {
            std::uniform_int_distribution<> dist(minv, maxv);
            return static_cast<T>(dist(__gen));
        } else if (std::is_floating_point_v<T>) {
            std::uniform_real_distribution<> dist(minv, maxv);
            return static_cast<T>(dist(__gen));
        }
    };
}  // namespace base
}  // namespace lux