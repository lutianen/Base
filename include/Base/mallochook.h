/**
 * @brief 重写 malloc 和 free，仅限于 Linux 使用
 * https://github.com/sjp38/mallochook/blob/master/mallochook.c
 * @usage
        1. Use hooked:
            LD_PRELOAD=$(PWD)/LuxUtils.so ./program
        2. Don't use hooked:
            ./program
 */

#pragma once

#ifdef __unix__

#include <dlfcn.h>

#include <cstdio>

namespace Lute {

/**
 * @brief
 *
 * @param size
 * @return void*
 */
inline void* malloc(size_t size) {
    using malloc_t = void* (*)(size_t size);
    static auto malloc_fn =
        reinterpret_cast<malloc_t>(dlsym(RTLD_NEXT, "malloc"));
    void* p = malloc_fn(size);
    fprintf(stderr, "\033[32mmalloc(%zu) = %p\033[0m\n", size, p);
    return p;
}

/**
 * @brief
 *
 * @param ptr
 */
inline void free(void* ptr) {
    using free_t = void (*)(void* ptr);
    static auto free_fn = reinterpret_cast<free_t>(dlsym(RTLD_NEXT, "free"));
    fprintf(stderr, "\033[31mfree(%p)\033[0m\n", ptr);
    free_fn(ptr);
}

#endif
}  // namespace Lute
