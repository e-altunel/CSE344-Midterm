#include <args_helper.h>
#include <test_macros.h>

#define PRINT(x) printf("%lu", x)
#define DESTROY(x)
#define NOT_EQUAL(x, y) x != y

int main()
{
  TEST_START(count_word_with_quotes, long unsigned int, const char *);

  SET_EXPECTED(0);
  TEST_ASSERT_EQUAL_PRINTABLE("hello");
  TEST_ASSERT_EQUAL_PRINTABLE("hello ");
  TEST_ASSERT_EQUAL_PRINTABLE("hello");
  TEST_ASSERT_EQUAL_PRINTABLE(" hello");
  TEST_ASSERT_EQUAL_PRINTABLE(" hello ");
  TEST_ASSERT_EQUAL_PRINTABLE("");

  SET_EXPECTED(7);
  TEST_ASSERT_EQUAL_PRINTABLE("\"hello\"");

  TEST_FINAL();
}