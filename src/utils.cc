#include <Base/currentThread.h>  // CurrentThread
#include <Base/fsUtils.h>        // readFile
#include <Base/utils.h>
#include <dirent.h>        // dirent
#include <pwd.h>           // passwd
#include <sys/resource.h>  // rlimit
#include <sys/times.h>     // tms
#include <unistd.h>        // sysconf _SC_CLK_TCK _SC_PAGE_SIZE

std::string Lute::toUpper(const std::string& str) {
    std::string rt = str;
    std::transform(rt.begin(), rt.end(), rt.begin(), ::toupper);
    return rt;
}

std::string Lute::toLower(const std::string& str) {
    std::string rt = str;
    std::transform(rt.begin(), rt.end(), rt.begin(), ::tolower);
    return rt;
}

std::string Lute::formatSI(int64_t s) {
    auto n = static_cast<double>(s);
    char buf[64];
    if (s < 1000)
        ::snprintf(buf, sizeof(buf), "%" PRId64, s);
    else if (s < 9995)
        ::snprintf(buf, sizeof(buf), "%.2fk", n / 1e3);
    else if (s < 99950)
        ::snprintf(buf, sizeof(buf), "%.1fk", n / 1e3);
    else if (s < 999500)
        ::snprintf(buf, sizeof(buf), "%.0fk", n / 1e3);
    else if (s < 9995000)
        ::snprintf(buf, sizeof(buf), "%.2fM", n / 1e6);
    else if (s < 99950000)
        ::snprintf(buf, sizeof(buf), "%.1fM", n / 1e6);
    else if (s < 999500000)
        ::snprintf(buf, sizeof(buf), "%.0fM", n / 1e6);
    else if (s < 9995000000)
        ::snprintf(buf, sizeof(buf), "%.2fG", n / 1e9);
    else if (s < 99950000000)
        ::snprintf(buf, sizeof(buf), "%.1fG", n / 1e9);
    else if (s < 999500000000)
        ::snprintf(buf, sizeof(buf), "%.0fG", n / 1e9);
    else if (s < 9995000000000)
        ::snprintf(buf, sizeof(buf), "%.2fT", n / 1e12);
    else if (s < 99950000000000)
        ::snprintf(buf, sizeof(buf), "%.1fT", n / 1e12);
    else if (s < 999500000000000)
        ::snprintf(buf, sizeof(buf), "%.0fT", n / 1e12);
    else if (s < 9995000000000000)
        ::snprintf(buf, sizeof(buf), "%.2fP", n / 1e15);
    else if (s < 99950000000000000)
        ::snprintf(buf, sizeof(buf), "%.1fP", n / 1e15);
    else if (s < 999500000000000000)
        ::snprintf(buf, sizeof(buf), "%.0fP", n / 1e15);
    else
        ::snprintf(buf, sizeof(buf), "%.2fE", n / 1e18);
    return buf;
}

std::string Lute::formatIEC(int64_t s) {
    auto n = static_cast<double>(s);
    char buf[64];
    const double Ki = 1024.0;
    const double Mi = Ki * 1024.0;
    const double Gi = Mi * 1024.0;
    const double Ti = Gi * 1024.0;
    const double Pi = Ti * 1024.0;
    const double Ei = Pi * 1024.0;

    if (n < Ki)
        ::snprintf(buf, sizeof buf, "%" PRId64, s);
    else if (n < Ki * 9.995)
        ::snprintf(buf, sizeof buf, "%.2fKi", n / Ki);
    else if (n < Ki * 99.95)
        ::snprintf(buf, sizeof buf, "%.1fKi", n / Ki);
    else if (n < Ki * 1023.5)
        ::snprintf(buf, sizeof buf, "%.0fKi", n / Ki);

    else if (n < Mi * 9.995)
        ::snprintf(buf, sizeof buf, "%.2fMi", n / Mi);
    else if (n < Mi * 99.95)
        ::snprintf(buf, sizeof buf, "%.1fMi", n / Mi);
    else if (n < Mi * 1023.5)
        ::snprintf(buf, sizeof buf, "%.0fMi", n / Mi);

    else if (n < Gi * 9.995)
        ::snprintf(buf, sizeof buf, "%.2fGi", n / Gi);
    else if (n < Gi * 99.95)
        ::snprintf(buf, sizeof buf, "%.1fGi", n / Gi);
    else if (n < Gi * 1023.5)
        ::snprintf(buf, sizeof buf, "%.0fGi", n / Gi);

    else if (n < Ti * 9.995)
        ::snprintf(buf, sizeof buf, "%.2fTi", n / Ti);
    else if (n < Ti * 99.95)
        ::snprintf(buf, sizeof buf, "%.1fTi", n / Ti);
    else if (n < Ti * 1023.5)
        ::snprintf(buf, sizeof buf, "%.0fTi", n / Ti);

    else if (n < Pi * 9.995)
        ::snprintf(buf, sizeof buf, "%.2fPi", n / Pi);
    else if (n < Pi * 99.95)
        ::snprintf(buf, sizeof buf, "%.1fPi", n / Pi);
    else if (n < Pi * 1023.5)
        ::snprintf(buf, sizeof buf, "%.0fPi", n / Pi);

    else if (n < Ei * 9.995)
        ::snprintf(buf, sizeof buf, "%.2fEi", n / Ei);
    else
        ::snprintf(buf, sizeof buf, "%.1fEi", n / Ei);
    return buf;
}

/// ----------------------
namespace Lute {
namespace detail {
    __thread int t_numOpenedFiles = 0;
    int fdDirFilter(const struct dirent* d) {
        if (::isdigit(d->d_name[0])) {
            ++t_numOpenedFiles;
        }
        return 0;
    }

    __thread std::vector<pid_t>* t_pids = nullptr;
    int taskDirFilter(const struct dirent* d) {
        if (::isdigit(d->d_name[0])) {
            t_pids->push_back(::atoi(d->d_name));
        }
        return 0;
    }

    int scanDir(const char* dirpath, int (*filter)(const struct dirent*)) {
        struct dirent** namelist = nullptr;
        int result = ::scandir(dirpath, &namelist, filter, alphasort);
        assert(namelist == nullptr);
        return result;
    }

    Timestamp g_startTime = Timestamp::now();
    // assume those won't change during the life time of a process.
    int g_clockTicks = static_cast<int>(::sysconf(_SC_CLK_TCK));
    int g_pageSize = static_cast<int>(::sysconf(_SC_PAGE_SIZE));
}  // namespace detail
}  // namespace Lute

pid_t Lute::ProcessInfo::pid() { return ::getpid(); }

std::string Lute::ProcessInfo::pidString() {
    char buf[32];
    snprintf(buf, sizeof buf, "%d", pid());
    return buf;
}

uid_t Lute::ProcessInfo::uid() { return ::getuid(); }

std::string Lute::ProcessInfo::username() {
    struct passwd pwd {};
    struct passwd* result = nullptr;
    char buf[8192];
    const char* name = "unknownuser";

    getpwuid_r(uid(), &pwd, buf, sizeof buf, &result);
    if (result) {
        name = pwd.pw_name;
    }
    return name;
}

uid_t Lute::ProcessInfo::euid() { return ::geteuid(); }

Lute::Timestamp Lute::ProcessInfo::startTime() { return detail::g_startTime; }

int Lute::ProcessInfo::clockTicksPerSecond() { return detail::g_clockTicks; }

int Lute::ProcessInfo::pageSize() { return detail::g_pageSize; }

bool Lute::ProcessInfo::isDebugBuild() {
#ifdef NDEBUG
    return false;
#else
    return true;
#endif
}

std::string Lute::ProcessInfo::hostname() {
    // HOST_NAME_MAX 64
    // _POSIX_HOST_NAME_MAX 255
    char buf[256];
    if (::gethostname(buf, sizeof buf) == 0) {
        buf[sizeof(buf) - 1] = '\0';
        return buf;
    } else {
        return "unknownhost";
    }
}

std::string Lute::ProcessInfo::procname() { return procname(procStat()); }

std::string Lute::ProcessInfo::procname(const std::string& stat) {
    std::string name;
    size_t lp = stat.find('(');
    size_t rp = stat.rfind(')');
    if (lp != std::string::npos && rp != std::string::npos && lp < rp) {
#if __cplusplus >= 201703L
        name = std::string_view(stat.data() + lp + 1, rp - lp - 1);
#else
        name = stat.substr(lp + 1, rp - lp - 1);
#endif
    }
    return name;
}

std::string Lute::ProcessInfo::procStatus() {
    std::string result;
    readFile("/proc/self/status", 65536, &result);
    return result;
}

std::string Lute::ProcessInfo::procStat() {
    std::string result;
    readFile("/proc/self/stat", 65536, &result);
    return result;
}

std::string Lute::ProcessInfo::threadStat() {
    char buf[64];
    snprintf(buf, sizeof buf, "/proc/self/task/%d/stat", CurrentThread::tid());
    std::string result;
    readFile(buf, 65536, &result);
    return result;
}

std::string Lute::ProcessInfo::exePath() {
    std::string result;
    char buf[1024];
    ssize_t n = ::readlink("/proc/self/exe", buf, sizeof buf);
    if (n > 0) {
        result.assign(buf, static_cast<std::basic_string<char>::size_type>(n));
    }
    return result;
}

int Lute::ProcessInfo::openedFiles() {
    detail::t_numOpenedFiles = 0;
    detail::scanDir("/proc/self/fd", detail::fdDirFilter);
    return detail::t_numOpenedFiles;
}

int Lute::ProcessInfo::maxOpenFiles() {
    struct rlimit rl {};
    if (::getrlimit(RLIMIT_NOFILE, &rl)) {
        return openedFiles();
    } else {
        return static_cast<int>(rl.rlim_cur);
    }
}

Lute::ProcessInfo::CpuTime Lute::ProcessInfo::cpuTime() {
    ProcessInfo::CpuTime t;
    struct tms tms {};
    if (::times(&tms) >= 0) {
        const auto hz = static_cast<double>(clockTicksPerSecond());
        t.userSeconds = static_cast<double>(tms.tms_utime) / hz;
        t.systemSeconds = static_cast<double>(tms.tms_stime) / hz;
    }
    return t;
}

int Lute::ProcessInfo::numThreads() {
    int result = 0;
    std::string status = procStatus();
    size_t pos = status.find("Threads:");
    if (pos != std::string::npos) {
        result = ::atoi(status.c_str() + pos + 8);
    }
    return result;
}

std::vector<pid_t> Lute::ProcessInfo::threads() {
    std::vector<pid_t> result;
    detail::t_pids = &result;
    detail::scanDir("/proc/self/task", detail::taskDirFilter);
    detail::t_pids = nullptr;
    std::sort(result.begin(), result.end());
    return result;
}
