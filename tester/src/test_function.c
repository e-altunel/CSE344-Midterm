#include <stdlib.h>
#include <string.h>

char *str_cpy(const char *src)
{
  size_t i = strlen(src);

  if (i == 0)
    return 0;

  char *dest = malloc(i + 1);
  if (dest == 0)
    return 0;

  strncpy(dest, src, i + 1);

  return dest;
}

int str_not_equal(const char *x, const char *y)
{
  if (x == y)
    return 0;
  else
    return strcmp(x, y) != 0;
}

void safe_free(void *ptr)
{
  if (ptr != 0)
    free(ptr);
}