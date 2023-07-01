#include <Base/string_view.h>
#include <Base/utils.h>

#include <iostream>

using namespace std;
using namespace Lute;

#define STR(x) #x
#define CHECK_STRING_VIEW(x, y)                        \
    printf("%s %s @ %s\n",                             \
           ((x) != (y)) ? ("[ " RED "Faild" CLR " ] ") \
                        : ("[ " GREEN "ok" CLR " ]"),  \
           STR(x), STR(y))

int main() {
#if __cplusplus >= 201103 && __cplusplus < 201703
    {
        string_view sv;
        CHECK_STRING_VIEW(sv.size(), 0);
        CHECK_STRING_VIEW(sv.empty(), true);
    }
    {
        string str{"Lute Polaris"};
        string_view sv(str);
        CHECK_STRING_VIEW(str.size(), sv.size());
        CHECK_STRING_VIEW(sv.empty(), false);
        CHECK_STRING_VIEW(str.length(), sv.length());

        CHECK_STRING_VIEW(sv.data(), str.data());

        CHECK_STRING_VIEW(sv.front(), str.front());
        CHECK_STRING_VIEW(sv.back(), str.back());

        CHECK_STRING_VIEW(sv[0], str[0]);
        CHECK_STRING_VIEW(sv[0], sv.at(0));
        CHECK_STRING_VIEW(sv.at(0), str.at(0));

        CHECK_STRING_VIEW(*sv.begin(), *str.begin());
        CHECK_STRING_VIEW(*sv.cbegin(), *str.cbegin());
        CHECK_STRING_VIEW(*sv.end(), *str.end());
        CHECK_STRING_VIEW(*sv.cend(), *str.cend());

        CHECK_STRING_VIEW(sv.substr(), str.substr());
        CHECK_STRING_VIEW(sv.substr(0), str.substr(0));
        CHECK_STRING_VIEW(sv.substr(0, 4), str.substr(0, 4));

        sv.clear();
        CHECK_STRING_VIEW(sv.empty(), true);

        sv.set(str.data());
        CHECK_STRING_VIEW(sv.empty(), false);
        CHECK_STRING_VIEW(sv.size(), str.size());

        sv.clear();
        sv.set(str.data(), str.size());
        CHECK_STRING_VIEW(sv.size(), str.size());

        sv.clear();
        sv.set(reinterpret_cast<const void*>(str.data()), str.size());
        CHECK_STRING_VIEW(sv.size(), str.size());

        sv.remove_prefix(1);
        CHECK_STRING_VIEW(sv.size(), str.size() - 1);
        CHECK_STRING_VIEW(*sv.begin(), *(str.begin() + 1));

        sv.remove_suffix(1);
        CHECK_STRING_VIEW(sv.size(), str.size() - 2);
        CHECK_STRING_VIEW(*sv.end(), *(str.end() - 1));

        string_view sv2(str);
        CHECK_STRING_VIEW(sv != sv2, true);

        sv.set(str.data());
        CHECK_STRING_VIEW(sv == sv2, true);

        CHECK_STRING_VIEW(sv.starts_with("Lute"), true);
        CHECK_STRING_VIEW(sv.starts_with('L'), true);
        CHECK_STRING_VIEW(sv.starts_with(sv2), true);

        CHECK_STRING_VIEW(sv.starts_with("Lute"), true);
        CHECK_STRING_VIEW(sv.starts_with('L'), true);
        CHECK_STRING_VIEW(sv.starts_with(sv2), true);

        CHECK_STRING_VIEW(sv.ends_with("Polaris"), true);
        CHECK_STRING_VIEW(sv.ends_with('s'), true);
        CHECK_STRING_VIEW(sv.ends_with(sv2), true);
    }

    {
        string_view sv("Lute Polaris");
        string_view sv2 = sv;

        PING(perf_memcmp);
        sv.starts_with("Lute");
        sv.starts_with('L');
        sv.starts_with(sv2);
        PONG(perf_memcmp);

        PING(perf_substr);
        sv.starts_with("Lute");
        sv.starts_with("Lute");
        sv.starts_with('L');
        sv.starts_with(sv2);
        PONG(perf_substr);
    }
    {
        auto a = "abc"_sv;
        std::cout << a << std::endl;
    }
#else
    cout << "C++17" << endl;
    auto a = "abc"_sv;
    std::cout << a << std::endl;
#endif
}
