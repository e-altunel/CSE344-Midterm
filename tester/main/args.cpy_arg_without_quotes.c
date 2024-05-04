#include <args.h>
#include <test_functions.h>
#include <test_macros.h>

#define PRINT(x) printf("%s", x)
#define DESTROY(x) safe_free(x)
#define NOT_EQUAL(x, y) str_not_equal(x, y)

int main()
{
  TEST_START(cpy_arg_without_quotes, char *, const char *);

  SET_EXPECTED(str_cpy("hello"));
  TEST_ASSERT_EQUAL_PRINTABLE("hello");
  TEST_ASSERT_EQUAL_PRINTABLE("hello ");
  TEST_ASSERT_EQUAL_PRINTABLE("hello world");

  SET_EXPECTED(0);
  TEST_ASSERT_EQUAL_PRINTABLE("");
  TEST_ASSERT_EQUAL_PRINTABLE(" ");
  TEST_ASSERT_EQUAL_PRINTABLE("\"hello\"");

  TEST_FINAL();
}