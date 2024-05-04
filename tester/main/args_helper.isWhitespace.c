#include <args_helper.h>
#include <test_macros.h>

#define PRINT(x) printf("%d", x)
#define DESTROY(x)
#define NOT_EQUAL(x, y) x != y

int main()
{
  TEST_START(isWhitespace, int, char);

  SET_EXPECTED(1);
  TEST_ASSERT_EQUAL_PRINTABLE(' ');
  TEST_ASSERT_EQUAL_PRINTABLE('\t');
  TEST_ASSERT_EQUAL_PRINTABLE('\n');

  SET_EXPECTED(0);
  TEST_ASSERT_EQUAL_PRINTABLE('a');
  TEST_ASSERT_EQUAL_PRINTABLE('b');
  TEST_ASSERT_EQUAL_PRINTABLE('\0');

  TEST_FINAL();
}