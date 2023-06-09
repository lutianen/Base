#include <cxxabi.h>    // abi
#include <execinfo.h>  // backtrace backtrace_symbols
#include <luxbase/currentThread.h>

namespace lux {
namespace base {
    namespace CurrentThread {
        __thread int t_cachedTid = 0;
        __thread char t_tidString[32];
        __thread int t_tidStringLength = 6;
        __thread const char* t_threadName = "unknown";
        static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");
    }  // namespace CurrentThread
}  // namespace base
}  // namespace lux

std::string lux::base::CurrentThread::stackTrace(bool demangle) {
    std::string stack;
    const int max_frames = 200;
    void* frame[max_frames];
    int nptrs = ::backtrace(frame, max_frames);
    char** strings = ::backtrace_symbols(frame, nptrs);
    if (strings) {
        size_t len = 256;
        char* demangled =
            demangle ? static_cast<char*>(::malloc(len)) : nullptr;

        // skipping the 0-th, which is this function
        for (int i = 1; i < nptrs; ++i) {
            if (demangle) {
                char* left_par = nullptr;
                char* plus = nullptr;
                for (char* p = strings[i]; *p; ++p) {
                    if (*p == '(')
                        left_par = p;
                    else if (*p == '+')
                        plus = p;
                }

                if (left_par && plus) {
                    *plus = '\0';
                    int status = 0;
                    char* ret = abi::__cxa_demangle(left_par + 1, demangled,
                                                    &len, &status);
                    *plus = '+';
                    if (status == 0) {
                        demangled = ret;  // ret could be realloc()
                        stack.append(strings[i], left_par + 1);
                        stack.append(demangled);
                        stack.append(plus);
                        stack.push_back('\n');
                        continue;
                    }
                }
            }
            // Fallback to mangled names
            stack.append(strings[i]);
            stack.push_back('\n');
        }
        free(demangled);
        free(strings);
    }
    return stack;
}
