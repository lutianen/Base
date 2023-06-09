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
#include <stdio.h>

namespace lux {
namespace base {
/**
* @brief
*
* @param size
* @return void*
*/
inline void *malloc(size_t size) {
    typedef void *(*malloc_t)(size_t size);
    static malloc_t malloc_fn = (malloc_t) dlsym(RTLD_NEXT, "malloc");
    void *p = malloc_fn(size);
    fprintf(stderr, "\033[32mmalloc(%zu) = %p\033[0m\n", size, p);
    return p;
}

/**
* @brief
*
* @param ptr
*/
inline void free(void *ptr) {
    typedef void (*free_t)(void *ptr);
    static free_t free_fn = (free_t) dlsym(RTLD_NEXT, "free");
    fprintf(stderr, "\033[31mfree(%p)\033[0m\n", ptr);
    free_fn(ptr);
}

#endif
}// namespace utils
}// namespace lux
