#include <args_helper.h>
#include <stdio.h>
#include <test_macros.h>

#define PRINT(x) printf("%lu", x)
#define DESTROY(x)
#define NOT_EQUAL(x, y) x != y

int main()
{
  TEST_START(count_args, long unsigned int, const char *);

  SET_EXPECTED(1);
  TEST_ASSERT_EQUAL_PRINTABLE("hello");
  TEST_ASSERT_EQUAL_PRINTABLE("hello ");
  TEST_ASSERT_EQUAL_PRINTABLE(" hello");
  TEST_ASSERT_EQUAL_PRINTABLE(" hello ");
  TEST_ASSERT_EQUAL_PRINTABLE("\"hello\"");
  TEST_ASSERT_EQUAL_PRINTABLE(" \"hello WORLD\" ");

  SET_EXPECTED(2);
  TEST_ASSERT_EQUAL_PRINTABLE("hello world");
  TEST_ASSERT_EQUAL_PRINTABLE("hello  world");
  TEST_ASSERT_EQUAL_PRINTABLE(" hello world");
  TEST_ASSERT_EQUAL_PRINTABLE(" hello  world");

  SET_EXPECTED(0);
  TEST_ASSERT_EQUAL_PRINTABLE("");
  TEST_ASSERT_EQUAL_PRINTABLE(" ");
  TEST_ASSERT_EQUAL_PRINTABLE("\"hello");
  TEST_ASSERT_EQUAL_PRINTABLE("hello\"");
  TEST_ASSERT_EQUAL_PRINTABLE("hello\" ");
  TEST_FINAL();
}