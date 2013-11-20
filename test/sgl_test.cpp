// sgl_win.cpp : Defines the entry point for the console application.
//

#include "../sgl.h"

#if defined(_WIN32)
#include <tchar.h>
#endif

#include <vector>  // For shootout purposes.

int get_random_int() {
    return 4;
}

#if defined(_WIN32)
#define BENCHMARK(expr) { \
    LARGE_INTEGER before, after; \
    QueryPerformanceCounter(&before); \
    (expr); \
    QueryPerformanceCounter(&after); \
    printf("Measure " #expr ": %ld\n", after.QuadPart - before.QuadPart); \
}
#elif defined(__linux__)
#define BENCHMARK(expr) { \
    long before, after;\
    before = sgl::get_nanoseconds();\
    (expr);\
    after = sgl::get_nanoseconds();\
    printf("Measure " #expr ": %ld\n", after - before); \
}
#endif

void stress_stl_vector(int reserve, int times) {
    std::vector<int> v;

    if (reserve)  v.reserve(reserve);

    for(int i = 0; i < times; ++i) {
        v.push_back(i);
    }
}

void stress_sgl_vector(int reserve, int times) {
    sgl::Array<int> v(reserve);

    for(int i = 0; i < times; ++i) {
        v.push_back(i);
    }
}

#if defined(_WIN32)
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char** argv)
#endif
{
    printf("Hello World\n");
#ifdef SGL_DEBUG
    printf("Debug build.\n");
#endif

    sgl::Maybe<int> maybe = get_random_int();
    sgl_expect(maybe.valid() && maybe.value() == get_random_int());
    sgl::Maybe<int> maybe_too(2);
    maybe = maybe_too;
    sgl_expect(maybe.value() == 2);

    size_t cache_size = sgl::cache_line_size();
    sgl_assert(cache_size);
    printf("cache line in bytes: %ld\n", cache_size);

    sgl::Array<int> v(1);
    for (int i = 0; i < 16; ++i) {
        v.push_back(i);
    }
    for (auto e : v) {
        printf("%d\n", e);
    }

    int reserve = 32;
    BENCHMARK(stress_sgl_vector(reserve, 1));
    BENCHMARK(stress_sgl_vector(reserve, 2));
    BENCHMARK(stress_sgl_vector(reserve, 4));
    BENCHMARK(stress_sgl_vector(reserve, 8));
    BENCHMARK(stress_sgl_vector(reserve, 16));
    BENCHMARK(stress_sgl_vector(reserve, 32));
    BENCHMARK(stress_sgl_vector(reserve, 128));
    BENCHMARK(stress_sgl_vector(reserve, 256));
    BENCHMARK(stress_sgl_vector(reserve, 512));
    BENCHMARK(stress_sgl_vector(reserve, 1000));
    BENCHMARK(stress_sgl_vector(reserve, 4000));
#if !defined(SGL_DEBUG)
    BENCHMARK(stress_sgl_vector(reserve, 16000));
    BENCHMARK(stress_sgl_vector(reserve, 100000));
    BENCHMARK(stress_sgl_vector(reserve, 200000));
#endif

    reserve = 32;
    BENCHMARK(stress_stl_vector(reserve, 1));
    BENCHMARK(stress_stl_vector(reserve, 2));
    BENCHMARK(stress_stl_vector(reserve, 4));
    BENCHMARK(stress_stl_vector(reserve, 8));
    BENCHMARK(stress_stl_vector(reserve, 16));
    BENCHMARK(stress_stl_vector(reserve, 32));
    BENCHMARK(stress_stl_vector(reserve, 128));
    BENCHMARK(stress_stl_vector(reserve, 256));
    BENCHMARK(stress_stl_vector(reserve, 512));
    BENCHMARK(stress_stl_vector(reserve, 1000));
    BENCHMARK(stress_stl_vector(reserve, 4000));
#if !defined(SGL_DEBUG)
    BENCHMARK(stress_stl_vector(reserve, 16000));
    BENCHMARK(stress_stl_vector(reserve, 100000));
    BENCHMARK(stress_stl_vector(reserve, 200000));
#endif

    {
        sgl::ScopedPtr<sgl::Array<int>> ad(new sgl::Array<int>(1));
        ad->push_back(42);
        sgl_expect((*ad)[0] == 42);
    }
    {
        sgl::Array<int> init_list = { 0, 1, 2, 3 };
        for (const auto& e : init_list) {
            printf("%d ", e);
        }
        sgl::dbgln(sgl::String());
    }

    {
        sgl::String s;
        printf("Empty string: [%s]\n", s.str());
    }
    {
        sgl::String s = "Hello World!";
        printf("Hello string: [%s]\n", s.str());
    }

    {
        sgl::String a = sgl::String("Hello, ").appended("appended World!");
        printf("%s\n", a.str());
        for (auto c : a) {
            printf("%c", c);
        }
        printf("\n");
    }
    {
        sgl::String lol = "LOL";
        for (int i = 0; i < 10; ++i) {
            sgl::dbg(lol);
        }
        sgl::dbgln(lol);
    }

    printf("Done.\n");

	return 0;
}