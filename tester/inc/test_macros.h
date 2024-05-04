#ifndef TESTER_INC_TEST_MACROS
#define TESTER_INC_TEST_MACROS

#include <stdio.h>

static int isPreviousTestPassed = 1;

#define PASSED()                                                                                                       \
  {                                                                                                                    \
    isPreviousTestPassed = 1;                                                                                          \
    fprintf(stdout, "\033[1;32m✓ \033[0m");                                                                            \
  }
#define FAILED()                                                                                                       \
  {                                                                                                                    \
    if (isPreviousTestPassed)                                                                                          \
      fprintf(stdout, "\n");                                                                                           \
    fprintf(stderr, "\033[1;31mAssertion failed: %s:%d: \033[0m\n", __FILE__, __LINE__);                               \
    fprintf(stderr, "\033[1;31mExpected: ");                                                                           \
    PRINT(tmp_expected);                                                                                               \
    fprintf(stderr, "\033[0m\n");                                                                                      \
    fprintf(stderr, "\033[1;31mActual: ");                                                                             \
    PRINT(tmp_actual);                                                                                                 \
    fprintf(stderr, "\033[0m\n");                                                                                      \
    isPreviousTestPassed = 0;                                                                                          \
  }

#define TEST_START(func, ret, ...)                                                                                     \
  ret tmp_expected            = 0;                                                                                     \
  ret tmp_actual              = 0;                                                                                     \
  ret (*fun_ptr)(__VA_ARGS__) = func;

#define TEST_FINAL()                                                                                                   \
  {                                                                                                                    \
    if (isPreviousTestPassed)                                                                                          \
    {                                                                                                                  \
      fprintf(stdout, "\n");                                                                                           \
      DESTROY(tmp_expected);                                                                                           \
    }                                                                                                                  \
    return 0;                                                                                                          \
  }

#define SET_EXPECTED(new_expected)                                                                                     \
  {                                                                                                                    \
    DESTROY(tmp_expected);                                                                                             \
    tmp_expected = new_expected;                                                                                       \
  }

#define TEST_ASSERT_EQUAL_PRINTABLE(...)                                                                               \
  {                                                                                                                    \
    tmp_actual = fun_ptr(__VA_ARGS__);                                                                                 \
    if (NOT_EQUAL(tmp_expected, tmp_actual))                                                                           \
    {                                                                                                                  \
      FAILED();                                                                                                        \
      DESTROY(tmp_actual);                                                                                             \
      DESTROY(tmp_expected);                                                                                           \
      return 1;                                                                                                        \
    }                                                                                                                  \
    PASSED();                                                                                                          \
    DESTROY(tmp_actual);                                                                                               \
  }

#endif /* TESTER_INC_TEST_MACROS */
