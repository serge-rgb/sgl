/**
* The MIT License (MIT)
*
* Copyright (c) 2013 Sergio Gonzalez
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#ifndef SGL_H_DEFINED
#define SGL_H_DEFINED

// C includes
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#include <sys/time.h>
#endif
#if defined(_WIN32)
#include <windows.h>
#endif
#if defined(__linux__)
#include<time.h>
#endif
#if defined(__linux__) || defined(__MACH__)
#include <unistd.h>
#endif

// C++ includes
#include <initializer_list>


// I don't like double negations
#ifndef SGL_DEBUG
#ifndef NDEBUG
#define SGL_DEBUG
#endif
#endif

#ifdef SGL_DEBUG
#ifndef sgl_assert
#define sgl_assert(expr) \
if (!(expr)) { \
    printf("%s:%d:0 Error: Assertion " #expr " failed\n", __FILE__, __LINE__);\
    exit(-1);\
}
#endif  // ifndef assert

#ifndef sgl_expect
#define sgl_expect(expr) \
if (!(expr)) { \
    printf("%s:%d:0 Warning: Expected " #expr ".\n", __FILE__, __LINE__);\
}
#endif
#else // Release:
#ifndef sgl_assert
#define sgl_assert(x);
#endif
#ifndef sgl_expect
#define sgl_expect(x);
#endif
#endif // ifdef SGL_DEBUG

namespace sgl {

////////////////////////////////////////////////////////////////////////////////
// Math functions.
////////////////////////////////////////////////////////////////////////////////

int64_t gcd(int64_t a, int64_t b) {
    sgl_assert(!((a == 0) && (b == 0)));
    if (a == 0) return b;
    return gcd(b%a, a);
}

int64_t lcm(int64_t a, int64_t b) {
    return a * b / gcd(a, b);
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Processor info
////////////////////////////////////////////////////////////////////////////////

/*
* Get the size of o L1 cache line.
* Returns 0 for platforms not yet supported.
*/
size_t cache_line_size()  {
#if defined(__linux__)
    FILE * p = 0;
    p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");
    size_t i = 0;
    if (p) {
        fscanf(p, "%zd", &i);
        fclose(p);
    }
    return i;
#elif defined(_WIN32)
    static size_t memoized_size = 0;
    if (memoized_size) {
        return memoized_size;
    }
    size_t size = 0;
    DWORD buffer_size = 0;
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = NULL;
    GetLogicalProcessorInformation(0, &buffer_size);
    buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*) malloc(buffer_size);
    GetLogicalProcessorInformation(&buffer[0], &buffer_size);
    for (DWORD i = 0; i != buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
        if (buffer && buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
            size = buffer[i].Cache.LineSize;
            break;
        }
    }
    free(buffer);
    memoized_size = size;
    return size;
#elif defined(__MACH__)
    return 64;  // What could possibly go wrong?
#else
    return 0;
#endif

}

long get_nanoseconds() {
#if defined(__linux__)
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return tp.tv_nsec;
#elif defined(__MACH__)
    struct timespec tp;
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    tp.tv_sec = mts.tv_sec;
    tp.tv_nsec = mts.tv_nsec;
    return tp.tv_nsec;
#endif
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Safety first:
////////////////////////////////////////////////////////////////////////////////

/*
 * Base non-copyable class.
 */
class Noncopyable {
public:
    Noncopyable() {}

private:
    Noncopyable(const Noncopyable&);
    Noncopyable& operator=(const Noncopyable&);
};

////////////////////////////////////////////////////////////////////////////////
// Output
////////////////////////////////////////////////////////////////////////////////
template<typename T>
void dbg(const T& that) {
    printf("%s", that.str());
}

template<typename T>
void dbg(const std::initializer_list<T>& list) {
    for(auto it = list.begin(); it != list.end(); it++) {
        dbg(*it);
        if(it != list.end() - 1) {
            printf(" ");
        }
    }
}

template<typename T>
void dbgln(const T& that) {
    printf("%s\n", that.str());
}

template<typename T>
void dbgln(const std::initializer_list<T>& list) {
    for(const auto& e: list) {
        dbgln(e);
    }
}

/**
 * Class to avoid exceptions and prevent NULL usage.
 *
 * Example:
 *
 * int parse_this_int(const string&);
 * ...
 * Maybe<int> i = parse_this_int(const string &str);
 *
 * if (i.valid()) {
 *     use_int(i.value());
 * } else {
 *     // handle parse error.
 * }
 */
template <typename T>
class Maybe {
public:
    Maybe() : m_is_valid(false) {}
    Maybe(const T& that) : m_value(that), m_is_valid(true) {}
    Maybe(const Maybe &that) : m_value(that.m_value), m_is_valid(that.m_is_valid) {}
    Maybe<T>& operator=(const Maybe<T>& that) {
        this->m_is_valid = that.m_is_valid;
        this->m_value = that.m_value;
        return *this;
    }

    bool valid() const { return m_is_valid; }

    /**
     * Usage:
     * if (m.valid()) some_function(m.value());
     */
    const T& value() const {
        sgl_assert(m_is_valid);
        return m_value;
    }

private:
    T m_value;
    bool m_is_valid;
};

/**
 * Simple scoped pointers. Call delete and delete[] respectively.
 */
template<typename T>
class ScopedPtr : public Noncopyable {
    public:
    ScopedPtr(T* ptr) : m_ptr(ptr) {}
    ScopedPtr(const T* ptr) : m_ptr(ptr) {}

    T& operator*() const { return *m_ptr; }
    T* operator->() const { return m_ptr; }

    T* get() const { m_ptr; }

    T* detach() {
        T* detached = m_ptr;
        m_ptr = NULL;
        return detached;
    }
private:
    ScopedPtr() {}
    ScopedPtr(T) {}
    T* m_ptr;

public:
    virtual ~ScopedPtr() {
        if (m_ptr) {
            delete m_ptr;
        }
    }
};

template<typename T>
class ScopedArray : public Noncopyable {
    public:
    ScopedArray(T* ptr) : m_ptr(ptr) {}
    ScopedArray(const T* ptr) : m_ptr(ptr) {}

    T& operator*() const { return *m_ptr; }
    T* operator->() const { return m_ptr; }

    T* get() const { m_ptr; }

    T* detach() {
        T* detached = m_ptr;
        m_ptr = NULL;
        return detached;
    }
private:
    ScopedArray() {}
    ScopedArray(T) {}
    T* m_ptr;
public:
    virtual ~ScopedArray() {
        if (m_ptr) {
            delete[] m_ptr;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Generic data structures
////////////////////////////////////////////////////////////////////////////////
/**
 * Array class designed to be faster than std::vector
 * Windows: push_back is faster in general but really kicks ass as the size grows.
 */
template <typename T>
class Array {
public:
    /**
     * Allocates space for at least num elements.
     */
    explicit Array(size_t reserve) :
        m_num_elements(0) {
        sgl_assert(reserve > 0);
        m_size = friendly_array_size(reserve);
        m_storage = new T[m_size];
    }

    Array(std::initializer_list<T> list) : m_num_elements(0) {
        sgl_assert(list.size() > 0);
        m_size = friendly_array_size(list.size());
        m_storage = new T[m_size];
        for (const auto e : list) {
            this->push_back(e);
        }
    }

    Array(const Array<T>& other) {
        this->m_size = other.m_size;
        this->m_num_elements = other.m_num_elements;
        this->m_storage = new T[m_size];
        memcpy(m_storage, other.m_storage, other.m_size);
    }

    T& operator[](size_t index) {
        sgl_assert(index < m_num_elements);
        return m_storage[index];
    }

    T* begin() const { return &m_storage[0]; }
    T* end() const { return &m_storage[0] + m_num_elements; }

    void push_back(const T& e) {
        m_num_elements++;
        if (m_num_elements * sizeof(T) > m_size) {  // Stretch
            m_size *= 2;
            T* new_storage = new T[m_size];
            memcpy(new_storage, m_storage, (m_num_elements - 1) * sizeof(T));
            delete[] m_storage;
            m_storage = new_storage;
        }
        m_storage[m_num_elements - 1] = e;
    }

    Array<T>& operator= (const Array<T>& other) {
        if (m_size < other.m_size) {
            if (m_storage) delete[] m_storage;
            m_storage = new T[other.m_size];
        }
        memcpy(m_storage, other.m_storage, other.m_size);
        m_size = other.m_size;
        return *this;
    }

    void resize(size_t num_elements) {
        sgl_expect(num_elements <= m_num_elements);
        m_num_elements = num_elements;
    }

    // Not virtual to avoid vtables. Assuming String is the only subclass.
    // and that compilers will call this (not in spec.)
    ~Array() {
        if (m_storage) {
            delete[] m_storage;
        }
    }

protected:
    inline size_t friendly_array_size(size_t min_num) {
        // Compute an array size that is a multiple of the cache line size.
        size_t line_size = cache_line_size();
        size_t type_size = sizeof(T);
        // Ceiling division of positive numbers:
        // line_size * ceil(min_num * type_size / line_size)
        return line_size * (1 + (((min_num * type_size) - 1) / line_size));
    }
    T* m_storage;
    size_t m_num_elements;
    size_t m_size;
};

/**
 * Functional-style string class
 * I expect this to be really slow without heavy compiler help, and even then..
 */
class String : public Array<char> {
public:
    String() : Array<char>(1) {
        m_storage[0] = '\0';
    }

    String(const char* str) : Array(strlen(str) + 1) {
        memcpy(m_storage, str, strlen(str));
        m_num_elements = strlen(str);
        m_storage[m_num_elements] = '\0';
    }

    String appended(const String& other) {
        const size_t new_storage = this->m_num_elements + other.m_num_elements;
        String new_string(new_storage);
        new_string.m_num_elements = this->m_num_elements + other.m_num_elements;
        sprintf(new_string.m_storage, "%s%s", this->m_storage, other.m_storage);
        return new_string;
    }

    const char* str() const {
        return m_storage;
    }

private:
    explicit String(size_t size) : Array(size) { m_storage[0] = '\0'; }
};

}  // namespace sgl
////////////////////////////////////////////////////////////////////////////////

#endif  // SGL_H_DEFINED

