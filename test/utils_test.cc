#include <Base/utils.h>  // toLower, toUpper PING PONG
#include <unistd.h>      // sleep

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

    const std::string s = "abcDefGH";

    std::cout << Lute::toLower(s) << std::endl;
    std::cout << Lute::toUpper(s) << std::endl;

    std::string s2 = "abcDefGH";
    Lute::toUpper(s2);
    std::cout << s2 << std::endl;
    Lute::toLower(s2);
    std::cout << s2 << std::endl;

    std::cout << RED "Hello Base" CLR << std::endl;
    std::cout << REDs("Hello Base") << std::endl;
    std::cout << GREEN "Hello Base" CLR << std::endl;
    std::cout << GREENs("Hello Base") << std::endl;
    std::cout << YELLOW "Hello Base" CLR << std::endl;
    std::cout << YELLOWs("Hello Base") << std::endl;
    std::cout << BLUE "Hello Base" CLR << std::endl;
    std::cout << BLUEs("Hello Base") << std::endl;
    std::cout << PURPLE "Hello Base" CLR << std::endl;
    std::cout << PURPLEs("Hello Base") << std::endl;
    std::cout << DEEPGREEN "Hello Base" CLR << std::endl;
    std::cout << DEEPGREENs("Hello Base") << std::endl;
    std::cout << WHITE "Hello Base" CLR << std::endl;
    std::cout << WHITEs("Hello Base") << std::endl;

    for (size_t i = 0; i < 10; ++i)
        std::cout << Lute::randomUniform<char, 1, 100>() << std::endl;
    std::cout << GREENs("--- char --") << std::endl;
    for (size_t i = 0; i < 10; ++i)
        std::cout << Lute::randomUniform<unsigned char, 1, 100>() << std::endl;
    std::cout << GREENs("--- unsigned char --") << std::endl;

    for (size_t i = 0; i < 10; ++i)
        std::cout << Lute::randomUniform<short, 1, 100>() << std::endl;
    std::cout << GREENs("--- short --") << std::endl;
    for (size_t i = 0; i < 10; ++i)
        std::cout << Lute::randomUniform<unsigned short, 1, 100>() << std::endl;
    std::cout << GREENs("--- unsigend short --") << std::endl;

    for (size_t i = 0; i < 10; ++i)
        std::cout << Lute::randomUniform<int, 1, 100>() << std::endl;
    std::cout << GREENs("--- int --") << std::endl;
    for (size_t i = 0; i < 10; ++i)
        std::cout << Lute::randomUniform<unsigned int, 1, 100>() << std::endl;
    std::cout << GREENs("--- unsigned int --") << std::endl;

    for (size_t i = 0; i < 10; ++i)
        std::cout << Lute::randomUniform<long, 1, 100>() << std::endl;
    std::cout << GREENs("--- long --") << std::endl;
    for (size_t i = 0; i < 10; ++i)
        std::cout << Lute::randomUniform<unsigned long, 1, 100>() << std::endl;
    std::cout << GREENs("--- unsigned long --") << std::endl;

    for (size_t i = 0; i < 10; ++i)
        std::cout << Lute::randomUniform<long long, 1, 100>() << std::endl;
    std::cout << GREENs("--- long long --") << std::endl;
    for (size_t i = 0; i < 10; ++i)
        std::cout << Lute::randomUniform<unsigned long long, 1, 100>()
                  << std::endl;
    std::cout << GREENs("--- unsigned long long --") << std::endl;
    for (size_t i = 0; i < 10; ++i) {
        std::cout << Lute::randomUniform<float, 1, 10>() << std::endl;
    }
    std::cout << GREENs("--- float --") << std::endl;
    for (size_t i = 0; i < 10; ++i) {
        std::cout << Lute::randomUniform<double, 1, 10>() << std::endl;
    }
    std::cout << GREENs("--- double --") << std::endl;

    for (size_t i = 0; i < 10000; i += 900) {
        std::cout << "SI: " << Lute::formatSI(i) << std::endl;
        std::cout << "IEC: " << Lute::formatIEC(i) << std::endl;
    }

    auto buf = new char[20];
    int v = 9527;
    int len = Lute::integer2Str<int>(buf, v);
    std::cout << "Len = " << len << ", buf = " << buf << std::endl;
    len = Lute::integer2StrHex(buf, v);
    std::cout << "Len = " << len << ", buf = " << buf << std::endl;

    return 0;
}
