#include <args.h>
#include <print_functions.h>
#include <test_functions.h>
#include <test_macros.h>

#define PRINT(x) print_str_array(x)
#define DESTROY(x) free_args(x)
#define NOT_EQUAL(x, y) str_array_not_equal(x, y)

int main()
{
  TEST_START(parse_args, char **, const char *);

  SET_EXPECTED(str_array_cpy((const char *[]){"hello", "world", NULL}));
  TEST_ASSERT_EQUAL_PRINTABLE("hello world");
  TEST_ASSERT_EQUAL_PRINTABLE("hello \"world\"");
  TEST_ASSERT_EQUAL_PRINTABLE(" \"hello\" world");

  SET_EXPECTED(0);
  TEST_ASSERT_EQUAL_PRINTABLE("");
  TEST_ASSERT_EQUAL_PRINTABLE(" ");
  TEST_ASSERT_EQUAL_PRINTABLE(" \"hello");

  SET_EXPECTED(str_array_cpy((const char *[]){"hello", "world", "!", NULL}));
  TEST_ASSERT_EQUAL_PRINTABLE("hello world !");
  TEST_ASSERT_EQUAL_PRINTABLE("hello \"world\" !");

  SET_EXPECTED(str_array_cpy((const char *[]){"hello", "world !", NULL}));
  TEST_ASSERT_EQUAL_PRINTABLE("hello \"world !\"");

  TEST_FINAL();
}