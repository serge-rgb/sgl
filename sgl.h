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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#if defined(_WIN32)
#include <windows.h>
#endif

// I don't like double negations
#ifndef NDEBUG
#define SGL_DEBUG
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
#if defined(_WIN32)
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
#endif
    return 0;
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

/**
 * Printable classes have a str method!
 */
class Printable {
    public:
        virtual char* str() = 0;
};

/**
 * Class to kill NULL usage
 *
 * Example:
 *
 * int parse_this_int(const string&);
 * ...
 * Maybe<int> i = parse_this_int(const string &str);
 *
 * if (x.valid()) {
 *     foo(x.value());
 * }
 */
template <typename T>
class Maybe {
public:
    Maybe() : m_is_valid(false) {}
    Maybe(const T& that) : m_value(that), m_is_valid(true) {}
    Maybe(const Maybe &that) : m_value(that.m_value), m_is_valid(that.m_is_valid) {}

    bool valid() const { return m_is_valid; }

    /**
     * Make sure to call valid() to guarantee that this isn't garbabe.
     */
    const T& value() const {
        sgl_assert(m_is_valid);
        return m_value;
    }

private:
    Maybe<T>& operator=(const Maybe<T>&);
    T m_value;
    bool m_is_valid;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Generic data structures
////////////////////////////////////////////////////////////////////////////////
/**
 * Vector class designed to be faster than std::vector
 */
template <typename T>
class Vector {
    public:
        /**
         * Allocates space for at least num elements.
         */
        explicit Vector(size_t reserve) :
            m_num_elements(0) {
            sgl_assert(reserve > 0);
            // Compute an array size that is a multiple of the cache line size.
            size_t line_size = cache_line_size();
            size_t type_size = sizeof(T);
            // Ceiling division of positive numbers:
            // line_size * ceil(reserve * type_size / line_size)
            m_size = line_size * (1 + (((reserve * type_size) - 1) / line_size));
            m_storage = new T[m_size];
        }

        T& operator[](size_t index) {
            sgl_assert(index < m_num_elements);
            return m_storage[index];
        }

        T* begin() const { return &m_storage[0]; }
        T* end() const { return &m_storage[0] + m_num_elements; }

        // TODO: make thread safe.
        void push_back(const T& e) {
            m_num_elements++;
            if (m_num_elements * sizeof(T) > m_size) {  // Stretch
                m_size *= m_factor;
                T* new_storage = new T[m_size];
                memcpy(new_storage, m_storage, (m_num_elements - 1) * sizeof(T));
                delete[] m_storage;
                m_storage = new_storage;
            }
            m_storage[m_num_elements - 1] = e;
        }

        virtual ~Vector() {
            if (m_storage) {
                delete[] m_storage;
            }
        }

    private:
        T* m_storage;
        size_t m_num_elements;
        size_t m_size;
        static size_t m_factor;  // How much do we resize the array when stretching.
};
template <typename T>
size_t Vector<T>::m_factor = 2;

}  // namespace sgl
////////////////////////////////////////////////////////////////////////////////

#endif  // SGL_H_DEFINED
