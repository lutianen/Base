#include <luxbase/utils.h>  // toLower, toUpper PING PONG
#include <unistd.h>         // sleep

#include <iostream>

void toTest() {
    for (int i = 0; i < 1E+8; i++) {
        (void)(i);
    }
}

void toTest2() { sleep(1); }

int main() {
    PING(test);
    toTest();
    PONG(test);

    PING(test2);
    toTest2();
    PONG(test2);

    std::string s = "abcDefGH";

    std::cout << lux::base::toLower(s) << std::endl;
    std::cout << lux::base::toUpper(s) << std::endl;

    std::cout << RED "Hello LuxBase" CLR << std::endl;
    std::cout << REDs("Hello LuxBase") << std::endl;
    std::cout << GREEN "Hello LuxBase" CLR << std::endl;
    std::cout << GREENs("Hello LuxBase") << std::endl;
    std::cout << YELLOW "Hello LuxBase" CLR << std::endl;
    std::cout << YELLOWs("Hello LuxBase") << std::endl;
    std::cout << BLUE "Hello LuxBase" CLR << std::endl;
    std::cout << BLUEs("Hello LuxBase") << std::endl;
    std::cout << PURPLE "Hello LuxBase" CLR << std::endl;
    std::cout << PURPLEs("Hello LuxBase") << std::endl;
    std::cout << DEEPGREEN "Hello LuxBase" CLR << std::endl;
    std::cout << DEEPGREENs("Hello LuxBase") << std::endl;
    std::cout << WHITE "Hello LuxBase" CLR << std::endl;
    std::cout << WHITEs("Hello LuxBase") << std::endl;

    for (size_t i = 0; i < 10; ++i)
        std::cout << lux::base::randomUniform<char, 1, 100>() << std::endl;
    std::cout << GREENs("--- char --") << std::endl;
    for (size_t i = 0; i < 10; ++i)
        std::cout << lux::base::randomUniform<unsigned char, 1, 100>()
                  << std::endl;
    std::cout << GREENs("--- unsigned char --") << std::endl;

    for (size_t i = 0; i < 10; ++i)
        std::cout << lux::base::randomUniform<short, 1, 100>() << std::endl;
    std::cout << GREENs("--- short --") << std::endl;
    for (size_t i = 0; i < 10; ++i)
        std::cout << lux::base::randomUniform<unsigned short, 1, 100>()
                  << std::endl;
    std::cout << GREENs("--- unsigend short --") << std::endl;

    for (size_t i = 0; i < 10; ++i)
        std::cout << lux::base::randomUniform<int, 1, 100>() << std::endl;
    std::cout << GREENs("--- int --") << std::endl;
    for (size_t i = 0; i < 10; ++i)
        std::cout << lux::base::randomUniform<unsigned int, 1, 100>()
                  << std::endl;
    std::cout << GREENs("--- unsigned int --") << std::endl;

    for (size_t i = 0; i < 10; ++i)
        std::cout << lux::base::randomUniform<long, 1, 100>() << std::endl;
    std::cout << GREENs("--- long --") << std::endl;
    for (size_t i = 0; i < 10; ++i)
        std::cout << lux::base::randomUniform<unsigned long, 1, 100>()
                  << std::endl;
    std::cout << GREENs("--- unsigned long --") << std::endl;

    for (size_t i = 0; i < 10; ++i)
        std::cout << lux::base::randomUniform<long long, 1, 100>() << std::endl;
    std::cout << GREENs("--- long long --") << std::endl;
    for (size_t i = 0; i < 10; ++i)
        std::cout << lux::base::randomUniform<unsigned long long, 1, 100>()
                  << std::endl;
    std::cout << GREENs("--- unsigned long long --") << std::endl;
    for (size_t i = 0; i < 10; ++i) {
        std::cout << lux::base::randomUniform<float, 1, 10>() << std::endl;
    }
    std::cout << GREENs("--- float --") << std::endl;
    for (size_t i = 0; i < 10; ++i) {
        std::cout << lux::base::randomUniform<double, 1, 10>() << std::endl;
    }
    std::cout << GREENs("--- double --") << std::endl;

    for (size_t i = 0; i < 1000000000; i+= 900) {
        std::cout << "SI: " << lux::base::formatSI(i) << std::endl;
        std::cout << "IEC: " <<lux::base::formatIEC(i) << std::endl;
    }

    return 0;
}
