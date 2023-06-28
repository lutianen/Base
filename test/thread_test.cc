#include <Base/currentThread.h>
#include <Base/utils.h>
#include <Base/thread.h>
#include <unistd.h>
#include <iostream>
#include <thread>

void mysleep(int seconds) {
    timespec t = {seconds, 0};
    nanosleep(&t, nullptr);
}

void threadFunc() { printf("tid=%d\n", Lute::CurrentThread::tid()); }

void threadFunc2(int x) {
    printf("tid=%d, x=%d\n", Lute::CurrentThread::tid(), x);
}

void threadFunc3() {
    printf("tid=%d\n", Lute::CurrentThread::tid());
    mysleep(1);
}

class Foo {
public:
    explicit Foo(double x) : x_(x) {}

    void memberFunc() {
        printf("tid=%d, Foo::x_=%f\n", Lute::CurrentThread::tid(), x_);
    }

    void memberFunc2(const std::string& text) {
        printf("tid=%d, Foo::x_=%f, text=%s\n", Lute::CurrentThread::tid(),
               x_, text.c_str());
    }

private:
    double x_;
};

int main() {
    printf("pid=%d, tid=%d\n", ::getpid(), Lute::CurrentThread::tid());

    Lute::Thread t1(threadFunc);
    t1.start();
    printf("t1.tid=%d\n", t1.tid());
    t1.join();

    Lute::Thread t2(std::bind(threadFunc2, 42),
                         "thread for free function with argument");
    t2.start();
    printf("t2.tid=%d\n", t2.tid());
    t2.join();

    Foo foo(87.53);
    Lute::Thread t3(std::bind(&Foo::memberFunc, &foo),
                         "thread for member function without argument");
    t3.start();
    t3.join();

    Lute::Thread t4(
        std::bind(&Foo::memberFunc2, std::ref(foo), std::string("lux")));
    t4.start();
    t4.join();

    {
        Lute::Thread t5(threadFunc3);
        t5.start();
        // t5 may destruct eariler than thread creation.
    }
    mysleep(2);
    {
        Lute::Thread t6(threadFunc3);
        t6.start();
        mysleep(2);
        // t6 destruct later than thread creation.
    }
    sleep(2);
    printf("number of created threads %d\n", Lute::Thread::numCreated());

    // std::cout << static_cast<pid_t>(std::this_thread::get_id()) << std::endl;
    std::cout << "getpid() - " << getpid() << std::endl;
    std::cout << "std::this_thread::get_id() - " << std::this_thread::get_id() << std::endl;
    std::cout << "pthread_self() - " << pthread_self() << std::endl;
}
