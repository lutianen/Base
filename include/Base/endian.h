/**
 *
 * @file endian.h
 * @brief 字节序转换
 *
 */
#pragma once

#include <byteswap.h>

#include <cstdint>
#include <type_traits>

#define LUTE_LITTLE_ENDIAN 1
#define LUTE_BIG_ENDIAN 2

/// @brief 字节序
#if BYTE_ORDER == LUTE_BIG_ENDIAN
#define LUTE_BYTE_ORDER LUTE_BIG_ENDIAN
#else
#define LUTE_BYTE_ORDER LUTE_LITTLE_ENDIAN
#endif

/**
 * std::enable_if<true, T>::type ---> T
 * std::enable_if<false, T>::type ---> 编译错误
 */

namespace Lute {

template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type byteswap(
    T val) {
    return static_cast<T>(bswap_64(static_cast<uint64_t>(val)));
}

template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type byteswap(
    T val) {
    return static_cast<T>(bswap_32(static_cast<uint32_t>(val)));
}

template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type byteswap(
    T val) {
    return static_cast<T>(bswap_32(static_cast<uint16_t>(val)));
}

#if LUTE_BYTE_ORDER == LUTE_BIG_ENDIAN
///
/// @brief 只在小端机器上执行 byteswap, 在大端机器上什么都不做
///
template <class T>
T byteswapOnLitteleEndian(T val) {
    return val;
}

///
/// @brief 只在大端机器上执行 byteswap, 在小端机器上什么都不做
///
template <class T>
T byteswapOnBigEndian(T val) {
    return byteswap(val);
}
#else

///
/// @brief 只在小端机器上执行 byteswap, 在大端机器上什么都不做
///
template <class T>
T byteswapOnLitteleEndian(T val) {
    return byteswap(val);
}

///
/// @brief 只在大端机器上执行 byteswap, 在小端机器上什么都不做
///
template <class T>
T byteswapOnBigEndian(T val) {
    return val;
}
#endif

}  // namespace Lute
