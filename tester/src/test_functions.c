#include <stdlib.h>
#include <string.h>

char *str_cpy(const char *src)
{
  size_t i;
  char  *dest;

  if (src == 0)
    return 0;

  i = strlen(src);
  if (i == 0)
    return 0;

  dest = malloc(i + 1);
  if (dest == 0)
    return 0;

  strncpy(dest, src, i + 1);

  return dest;
}

char **str_array_cpy(const char **src)
{
  size_t i = 0;
  char **dest;

  while (src[i] != 0)
    i++;

  dest = malloc(sizeof(char *) * (i + 1));
  if (dest == 0)
    return 0;

  i = 0;
  while (src[i] != 0)
  {
    dest[i] = str_cpy(src[i]);
    if (dest[i] == 0)
    {
      while (i > 0)
        free(dest[--i]);
      free(dest);
      return 0;
    }
    i++;
  }

  dest[i] = 0;

  return dest;
}

int str_not_equal(const char *x, const char *y)
{
  if (x == y)
    return 0;
  else
    return strcmp(x, y) != 0;
}

int str_array_not_equal(const char **x, const char **y)
{
  size_t i = 0;

  if (x == y)
    return 0;

  while (x[i] != 0 && y[i] != 0)
  {
    if (str_not_equal(x[i], y[i]))
      return 1;
    i++;
  }

  return x[i] != y[i];
}

void safe_free(void *ptr)
{
  if (ptr != 0)
    free(ptr);
}
