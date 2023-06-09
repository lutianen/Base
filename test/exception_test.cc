#include <luxbase/currentThread.h>
#include <luxbase/exception.h>

#include <functional>
#include <vector>

class Bar {
public:
    void test(std::vector<std::string> names = {}) {
        (void)(names);
        printf("Stack:\n%s\n",
               lux::base::CurrentThread::stackTrace(true).c_str());
        [] {
            printf("Stack inside lambda:\n%s\n",
                   lux::base::CurrentThread::stackTrace(true).c_str());
        }();
        std::function<void()> func([] {
            printf("Stack inside std::function:\n%s\n",
                   lux::base::CurrentThread::stackTrace(true).c_str());
        });
        func();

        func = std::bind(&Bar::callback, this);
        func();

        throw lux::base::Exception("oops");
    }

private:
    void callback() {
        printf("Stack inside std::bind:\n%s\n",
               lux::base::CurrentThread::stackTrace(true).c_str());
    }
};

void foo() {
    Bar b;
    b.test();
}

int main() {
    try {
        foo();
    } catch (const lux::base::Exception& ex) {
        printf("reason: %s\n", ex.what());
        printf("stack trace:\n%s\n", ex.stackTrace());
    }
}