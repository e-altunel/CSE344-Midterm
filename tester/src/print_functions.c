#include <stdio.h>
#include <sys/types.h>

void print_str_array(const char **str_array)
{
  size_t i = 0;

  printf("\n");
  while (str_array[i] != 0)
  {
    printf("%s\n", str_array[i]);
    i++;
  }
}