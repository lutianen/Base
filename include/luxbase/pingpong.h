/**
 * @brief Simple macro to measure time of execution of a piece of code
 * @usage
 *     PING(CodeToBeMeasured)
 *     CodeToBeMeasure
 *     PONG(CodeToBeMeasured)
 */

#pragma once

#include <chrono>
#include <iostream>

namespace lux {
namespace base {

#define PING(x) auto bench_##x = std::chrono::steady_clock::now();
#define PONG(x)                                                             \
    std::cout << #x ": "                                                    \
              << std::chrono::duration_cast<std::chrono::duration<double>>( \
                     std::chrono::steady_clock::now() - bench_##x)          \
                     .count()                                               \
              << "s" << std::endl;

}  // namespace base
}  // namespace lux
