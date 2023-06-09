#include <luxbase/pingpong.h>
#include <unistd.h>

void toTest() {
    for (int i = 0; i < 1E+8; i++) {
        (void)(i);
    }
}

void toTest2() {
    sleep(1);
}

int main() {
    PING(test);
    toTest();
    PONG(test);
    
    PING(test2);
    toTest2();
    PONG(test2);

    return 0;
}
