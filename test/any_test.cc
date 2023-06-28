#include <Base/any.h>

#include <iostream>
#include <memory>

#define CHECK(x)                                                              \
    ((x) ? (void)(0)                                                          \
         : (void(fprintf(stdout, "Failed at %d:%s: %s\n", __LINE__, __FILE__, \
                         #x)),                                                \
            std::exit(EXIT_FAILURE)))

template <size_t N>
struct words {
    void* w[N];
};

struct big_type {
    char i_wanna_be_big[256];
    std::string value;

    big_type() : value(std::string(300, 'b')) {
        i_wanna_be_big[0] = i_wanna_be_big[50] = 'k';
    }

    bool check() {
        CHECK(value.size() == 300);
        CHECK(value.front() == 'b' && value.back() == 'b');
        CHECK(i_wanna_be_big[0] == 'k' && i_wanna_be_big[50] == 'k');
        return true;
    }
};

// small type which has nothrow move ctor but throw copy ctor
struct regression1_type {
    const void* confuse_stack_storage = reinterpret_cast<void*>(0);
    regression1_type() {}
    regression1_type(const regression1_type&) {}
    regression1_type(regression1_type&&) noexcept {}
    regression1_type& operator=(const regression1_type&) { return *this; }
    regression1_type& operator=(regression1_type&&) { return *this; }
};

int main() {
#if __cplusplus >= 201703L
    std::cout << std::boolalpha;

    // any type
    Lute::any a = 1;
    std::cout << a.type().name() << ": " << Lute::any_cast<int>(a) << '\n';
    a = 3.14;
    std::cout << a.type().name() << ": " << Lute::any_cast<double>(a) << '\n';
    a = true;
    std::cout << a.type().name() << ": " << Lute::any_cast<bool>(a) << '\n';

    // bad cast
    try {
        a = 1;
        std::cout << Lute::any_cast<float>(a) << '\n';
    } catch (const Lute::bad_any_cast& e) {
        std::cout << e.what() << '\n';
    }

    // has value
    a = 2;
    if (a.has_value()) {
        std::cout << a.type().name() << ": " << Lute::any_cast<int>(a) << '\n';
    }

    // reset
    a.reset();
    if (!a.has_value()) {
        std::cout << "no value\n";
    }

    // pointer to contained data
    a = 3;
    int* i = Lute::any_cast<int>(&a);
    std::cout << *i << "\n";
#else

    {
        Lute::any x = 4;
        Lute::any y = big_type();
        Lute::any z = 6;

        CHECK(Lute::any().empty());
        CHECK(!Lute::any(1).empty());
        CHECK(!Lute::any(big_type()).empty());

        CHECK(!x.empty() && !y.empty() && !z.empty());
        y.clear();
        CHECK(!x.empty() && y.empty() && !z.empty());
        x = y;
        CHECK(x.empty() && y.empty() && !z.empty());
        z = Lute::any();
        CHECK(x.empty() && y.empty() && z.empty());
    }

    {
        Lute::any i4 = 4;
        Lute::any i5 = 5;
        Lute::any f6 = 6.0f;
        Lute::any big1 = big_type();
        Lute::any big2 = big_type();
        Lute::any big3 = big_type();

        CHECK(Lute::any_cast<int>(&i4) != nullptr);
        CHECK(Lute::any_cast<float>(&i4) == nullptr);
        CHECK(Lute::any_cast<int>(i5) == 5);
        CHECK(Lute::any_cast<float>(f6) == 6.0f);
        CHECK(Lute::any_cast<big_type>(big1).check() &&
              Lute::any_cast<big_type>(big2).check() &&
              Lute::any_cast<big_type>(big3).check());
    }

    {
        std::shared_ptr<int> ptr_count(new int);
        std::weak_ptr<int> weak = ptr_count;
        Lute::any p0 = 0;

        CHECK(weak.use_count() == 1);
        Lute::any p1 = ptr_count;
        CHECK(weak.use_count() == 2);
        Lute::any p2 = p1;
        CHECK(weak.use_count() == 3);
        p0 = p1;
        CHECK(weak.use_count() == 4);
        p0 = 0;
        CHECK(weak.use_count() == 3);
        p0 = std::move(p1);
        CHECK(weak.use_count() == 3);
        p0.swap(p1);
        CHECK(weak.use_count() == 3);
        p0 = 0;
        CHECK(weak.use_count() == 3);
        p1.clear();
        CHECK(weak.use_count() == 2);
        p2 = Lute::any(big_type());
        CHECK(weak.use_count() == 1);
        p1 = ptr_count;
        CHECK(weak.use_count() == 2);
        ptr_count = nullptr;
        CHECK(weak.use_count() == 1);
        p1 = Lute::any();
        CHECK(weak.use_count() == 0);
    }

    {
        auto is_stack_allocated = [](const Lute::any& a, const void* obj1) {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
            uintptr_t a_ptr = (uintptr_t)(&a);
            uintptr_t obj = (uintptr_t)(obj1);
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
            uintptr_t a_ptr = (uintptr_t)(&a);
            uintptr_t obj = (uintptr_t)(obj1);
#pragma GCC diagnostic pop

#endif
            return (obj >= a_ptr && obj < a_ptr + sizeof(Lute::any));
        };

        // static_assert(sizeof(std::unique_ptr<big_type>) <= sizeof(void*) * 1,
        // "unique_ptr too big");
        static_assert(sizeof(std::shared_ptr<big_type>) <= sizeof(void*) * 2,
                      "shared_ptr too big");

        Lute::any i = 400;
        Lute::any f = 400.0f;
        // Lute::any unique = std::unique_ptr<big_type>(); -- must be copy
        // constructible
        Lute::any shared = std::shared_ptr<big_type>();
        Lute::any rawptr = static_cast<void*>(nullptr);
        Lute::any big = big_type();
        Lute::any w2 = words<2>();
        Lute::any w3 = words<3>();

        CHECK(is_stack_allocated(i, Lute::any_cast<int>(&i)));
        CHECK(is_stack_allocated(f, Lute::any_cast<float>(&f)));
        CHECK(is_stack_allocated(rawptr, Lute::any_cast<void*>(&rawptr)));
        // CHECK(is_stack_allocated(unique,
        // Lute::any_cast<std::unique_ptr<big_type>>(&unique)));
        CHECK(is_stack_allocated(
            shared, Lute::any_cast<std::shared_ptr<big_type> >(&shared)));
        CHECK(!is_stack_allocated(big, Lute::any_cast<big_type>(&big)));
        CHECK(is_stack_allocated(w2, Lute::any_cast<words<2> >(&w2)));
        CHECK(!is_stack_allocated(w3, Lute::any_cast<words<3> >(&w3)));

        Lute::any r1 = regression1_type();
        CHECK(
            is_stack_allocated(r1, Lute::any_cast<const regression1_type>(&r1)));
    }

    // correctly stored decayed and retrieved in decayed form
    {
        const int i = 42;
        Lute::any a = i;

        // retrieve
        CHECK(Lute::any_cast<int>(&a) != nullptr);
        CHECK(Lute::any_cast<const int>(&a) != nullptr);

#ifndef ANY_IMPL_NO_EXCEPTIONS
        // must not throw
        bool except1 = false, except2 = false, except3 = false;

        // same with reference to any
        try {
            Lute::any_cast<int>(a);
        } catch (const Lute::bad_any_cast&) {
            except1 = true;
        }
        try {
            Lute::any_cast<const int>(a);
        } catch (const Lute::bad_any_cast&) {
            except2 = true;
        }
        try {
            Lute::any_cast<const int>(std::move(a));
        } catch (const Lute::bad_any_cast&) {
            except3 = true;
        }

        CHECK(except1 == false);
        CHECK(except2 == false);
        CHECK(except3 == false);
#endif
    }
#endif
}
