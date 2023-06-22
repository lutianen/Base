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

    return 0;
}
