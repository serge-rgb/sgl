/**
 *  TODO: add a license thingy here
 */

#ifndef SGL_H_DEFINED
#define SGL_H_DEFINED

#include <stdio.h>
#include <stdlib.h>

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
#else // DEBUG
#ifndef sgl_assert
#define sgl_assert(x);
#endif
#define sgl_expect(x);
#endif // ifdef SGL_DEBUG

namespace sgl {

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
 * auto x = Maybe<int> parse_this_int(const string &str);
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
     * Make sure to call IsValid() to guarantee that this isn't garbabe.
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
 * Vector class designed to be faster than std::vector
 */
template <typename T>
class Vector : public Noncopyable {
    public:
        Vector() {
        }
        /**
         * Allocates space for at least count elements.
         * Adds count elements.
         */
        Vector(size_t count) {
            /**
             * TODO: smart allocation.
             */
            m_storage = new T[count];
            m_count = count;
            m_num_elements = count;
        }
        T& operator[](size_t index) {
            sgl_assert(index < m_count);
            return m_storage[index];
        }
        T* begin() const { return m_storage; }
        T* end() const { return &m_storage[m_count - 1]; }
    private:
        T* m_storage;
        size_t m_num_elements;
        size_t m_count;
};

}  // namespace sgl

#endif  // SGL_H_DEFINED
