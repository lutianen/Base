#include <LuteBase.h>

#define INI_FILE_NAME "conf/iniConfigTest.ini"

#define LUTE_INI Lute::SingletonPtr<Lute::ini::INI>::GetInstance(INI_FILE_NAME)
#define LUTE_INI_CONTENT \
    Lute::SingletonPtr<Lute::ini::INIStructure>::GetInstance()

#define LUTE_INI_TYPE(x) x##_sv
/// Assert that `x` is `Lute::string_view`
#ifdef NDEBUG
#define LUTE_INI_ASSERT(x) (void)x
#else
#define LUTE_INI_ASSERT(x)                                            \
    do {                                                              \
        if (!(std::is_same<decltype(x), Lute::string_view>::value)) { \
            ::fprintf(stderr,                                         \
                      "Assertion failed: %s:%d `" #x                  \
                      "` must be `Lute::string_view`!\n",             \
                      __FILE__, __LINE__);                            \
            ::abort();                                                \
        }                                                             \
    } while (0)
#endif

/// Read section / key
#define LUTE_INI_READ(SECTION, KEY)                            \
    [&]() {                                                    \
        LUTE_INI_ASSERT(LUTE_INI_TYPE(SECTION));               \
        LUTE_INI_ASSERT(LUTE_INI_TYPE(KEY));                   \
        LUTE_INI->read(*LUTE_INI_CONTENT);                     \
        return Lute::string_view(                              \
            (*LUTE_INI_CONTENT)[LUTE_INI_TYPE(SECTION).data()] \
                               [LUTE_INI_TYPE(KEY).data()]);   \
    }()

/// Add or Update section / key-value and save
#define LUTE_INI_WRITE(SECTION, KEY, VALUE)                 \
    do {                                                    \
        LUTE_INI_ASSERT(LUTE_INI_TYPE(SECTION));            \
        LUTE_INI_ASSERT(LUTE_INI_TYPE(KEY));                \
        LUTE_INI_ASSERT(LUTE_INI_TYPE(VALUE));              \
        (*LUTE_INI_CONTENT)[LUTE_INI_TYPE(SECTION).data()]  \
                           [LUTE_INI_TYPE(KEY).data()] =    \
                               LUTE_INI_TYPE(VALUE).data(); \
        LUTE_INI->write(*LUTE_INI_CONTENT);                 \
    } while (0)

namespace Lute {
namespace ini {
    ///
    /// @brief Initialize ini config file
    ///
    inline void initIniConfig(const Lute::string_view& iniFileName) {
        if (!Lute::FSUtil::touch(std::string(iniFileName.data()))) return;
        LUTE_INI->generate(*LUTE_INI_CONTENT);

        if (Lute::FSUtil::fileSize(std::string(iniFileName.data())) == 0) {
            LUTE_INI_WRITE("persion", "body", "Lute");
            LUTE_INI_WRITE("money", "RMB", "100");
            LUTE_INI_WRITE("money", "Dollar", "13.87");
        }
    }

}  // namespace ini
}  // namespace Lute

int main() {
    Lute::ini::initIniConfig(INI_FILE_NAME);

    PING(lambda);
    std::cout << LUTE_INI_READ("person", "body") << std::endl;
    std::cout << LUTE_INI_READ("money", "RMB") << std::endl;
    std::cout << LUTE_INI_READ("money", "Dollar") << std::endl;
    PONG(lambda);

    LUTE_INI_WRITE("person", "body", "Lutianen");

    return 0;
}