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

    return 0;
}
