/* Copyright (C) 2016-2017 INRA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef ORG_VLEPROJECT_UNIT_TEST_HPP
#define ORG_VLEPROJECT_UNIT_TEST_HPP

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace unit_test {
namespace detail {

struct tester
{
    int errors = 0;
    bool called_report_function = false;

    tester& operator++() noexcept
    {
        errors++;

        return *this;
    }

    ~tester() noexcept
    {
        if (called_report_function == false) {
            std::cerr << "\n======================================\n"
                      << "unit_test::report_errors() not called.\n\nUsage:\n"
                      << "int main(int argc, char*[] argc)\n"
                      << "{\n"
                      << "    [...]\n"
                      << "    return unit_test::report_errors();\n"
                      << "}\n";
            std::abort();
        }
    }
};

inline tester&
test_errors()
{
    static tester t;

    return t;
}

inline void
ensures_impl(const char* expr,
             const char* file,
             int line,
             const char* function)
{
    std::cerr << file << "(" << line << "): test '" << expr
              << "' failed in function '" << function << "'\n";

    ++test_errors();
}

inline void
ensures_equal_impl(const char* expr1,
                   const char* expr2,
                   const char* file,
                   int line,
                   const char* function)
{
    std::cerr << file << "(" << line << "): test '" << expr1 << " == " << expr2
              << "' failed in function '" << function << "'\n";

    ++test_errors();
}

inline void
ensures_not_equal_impl(const char* expr1,
                       const char* expr2,
                       const char* file,
                       int line,
                       const char* function)
{
    std::cerr << file << "(" << line << "): test '" << expr1 << " != " << expr2
              << "' failed in function '" << function << "'\n";

    ++test_errors();
}

inline void
ensures_throw_impl(const char* excep,
                   const char* file,
                   int line,
                   const char* function)
{
    std::cerr << file << "(" << line << "): exception '" << excep
              << "' throw failed in function '" << function << "'\n";

    ++test_errors();
}

inline void
ensures_not_throw_impl(const char* excep,
                       const char* file,
                       int line,
                       const char* function)
{
    std::cerr << file << "(" << line << "): exception '" << excep
              << "' not throw failed in function '" << function << "'\n";

    ++test_errors();
}

inline void
ensures_approximately_equal(const char* expr1,
                            const char* expr2,
                            const char* epsilon,
                            const char* file,
                            int line,
                            const char* function)
{
    std::cerr << file << "(" << line << "): test '" << expr1 << " != " << expr2
              << " (epsilon: " << epsilon << ")"
              << "' failed in function '" << function << "'\n";

    ++test_errors();
}

inline void
ensures_not_reached(const char* file, int line, const char* function)
{
    std::cerr << file << "(" << line << "): reached in function " << function
              << "'\n";

    ++test_errors();
}

} // namespace details

inline int
report_errors()
{
    auto& tester = unit_test::detail::test_errors();
    tester.called_report_function = true;
    int errors = tester.errors;

    if (errors == 0)
        std::cerr << "No errors detected.\n";
    else
        std::cerr << errors << " error" << (errors == 1 ? "" : "s")
                  << " detected.\n";

    return errors;
}

} // namespace unit_test

#define Ensures(expr)                                                         \
    do {                                                                      \
        if (not(expr)) {                                                      \
            unit_test::detail::ensures_impl(                                  \
              #expr, __FILE__, __LINE__, __func__);                           \
        }                                                                     \
    } while (0)

#define EnsuresEqual(expr1, expr2)                                            \
    do {                                                                      \
        if (not((expr1) == (expr2))) {                                        \
            unit_test::detail::ensures_equal_impl(                            \
              #expr1, #expr2, __FILE__, __LINE__, __func__);                  \
        }                                                                     \
    } while (0)

#define EnsuresNotEqual(expr1, expr2)                                         \
    do {                                                                      \
        if (not((expr1) != (expr2))) {                                        \
            unit_test::detail::ensures_not_equal_impl(                        \
              #expr1, #expr2, __FILE__, __LINE__, __func__);                  \
        }                                                                     \
    } while (0)

#define EnsuresThrow(expr, Excep)                                             \
    do {                                                                      \
        try {                                                                 \
            expr;                                                             \
            unit_test::detail::ensures_throw_impl(                            \
              #Excep, __FILE__, __LINE__, __func__);                          \
        } catch (const Excep&) {                                              \
        } catch (...) {                                                       \
            unit_test::detail::ensures_throw_impl(                            \
              #Excep, __FILE__, __LINE__, __func__);                          \
        }                                                                     \
    } while (0)

#define EnsuresNotThrow(expr, Excep)                                          \
    do {                                                                      \
        try {                                                                 \
            expr;                                                             \
        } catch (const Excep&) {                                              \
            unit_test::detail::ensures_not_throw_impl(                        \
              #Excep, __FILE__, __LINE__, __func__);                          \
        } catch (...) {                                                       \
        }                                                                     \
    } while (0)

#define EnsuresApproximatelyEqual(expr1, expr2, epsilon)                      \
    do {                                                                      \
        if (not(std::abs((expr1) - (expr2)) <=                                \
                ((std::abs(expr1) < std::abs(expr2) ? std::abs(expr2)         \
                                                    : std::abs(expr1)) *      \
                 (epsilon)))) {                                               \
            unit_test::detail::ensures_approximately_equal(                   \
              #expr1, #expr2, #epsilon, __FILE__, __LINE__, __func__);        \
        }                                                                     \
    } while (0)

#define EnsuresEssentiallyEqual(expr1, expr2, epsilon)                        \
    do {                                                                      \
        if (not(std::abs((expr1) - (expr2)) <=                                \
                ((std::abs(expr1) > std::abs(expr2) ? std::abs(expr2)         \
                                                    : std::abs(expr1)) *      \
                 (epsilon)))) {                                               \
            unit_test::detail::ensures_approximately_equal(                   \
              #expr1, #expr2, #epsilon, __FILE__, __LINE__, __func__);        \
        }                                                                     \
    } while (0)

#define EnsuresNotReached()                                                   \
    do {                                                                      \
        unit_test::detail::ensures_not_reached(__FILE__, __LINE__, __func__); \
    } while (0)

#endif // #ifndef ORG_VLEPROJECT_UNIT_TEST_HPP
