#ifndef TESTER_INC_TEST_MACROS
#define TESTER_INC_TEST_MACROS

#include <stdio.h>

static int isPreviousTestPassed = 1;

#define PASSED()                                                                                                       \
  {                                                                                                                    \
    isPreviousTestPassed = 1;                                                                                          \
    fprintf(stdout, "\033[1;32m✓ \033[0m");                                                                            \
  }
#define FAILED(format, ...)                                                                                            \
  {                                                                                                                    \
    if (isPreviousTestPassed)                                                                                          \
      fprintf(stdout, "\n");                                                                                           \
    fprintf(stderr, "\033[1;31mAssertion failed: %s:%d: " format "\033[0m\n", __FILE__, __LINE__, ##__VA_ARGS__);      \
    return 1;                                                                                                          \
  }
#define TEST_START()
#define TEST_FINAL()                                                                                                   \
  {                                                                                                                    \
    if (isPreviousTestPassed)                                                                                          \
      fprintf(stdout, "\n");                                                                                           \
    return 0;                                                                                                          \
  }

#define TEST_ASSERT_EQUAL_PRINTABLE(expected, actual, format)                                                          \
  {                                                                                                                    \
    if (expected != actual)                                                                                            \
    {                                                                                                                  \
      FAILED("expected: " format ", actual: " format, expected, actual);                                               \
    }                                                                                                                  \
    PASSED();                                                                                                          \
  }

#endif /* TESTER_INC_TEST_MACROS */
