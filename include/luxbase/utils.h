/**
 * @brief
 */

#pragma once

#include <algorithm>  // toupper
#include <string>     // string
#include <chrono>     // chrono
#include <iostream>   // cout endl

namespace lux {
namespace base {

/**
 * @brief Simple macro to measure time of execution of a piece of code
 * @usage
 *     PING(CodeToBeMeasured)
 *     CodeToBeMeasure
 *     PONG(CodeToBeMeasured)
 */
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
}  // namespace base
}  // namespace lux