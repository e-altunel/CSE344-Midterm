#include <args_helper.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

char *cpy_arg_without_quotes(const char *src)
{
  size_t i = count_word_without_quotes(src);
  char  *dest;
  if (i == 0)
    return 0;

  dest = (char *)malloc(i + 1);
  if (dest == 0)
    return 0;

  strncpy(dest, src, i);
  dest[i] = '\0';

  return dest;
}
char *cpy_arg_with_quotes(const char *src)
{
  size_t i = count_word_with_quotes(src);
  char  *dest;

  if (i < 2)
    return 0;

  dest = (char *)malloc(i - 1);
  if (dest == 0)
    return 0;

  strncpy(dest, src + 1, i - 2);
  dest[i - 2] = '\0';

  return dest;
}
#include <stdio.h>
char **parse_args(const char *line)
{
  size_t arg_count;
  size_t arg_index;
  char **args;
  char  *tmp_str;
  size_t i;

  arg_count = count_args(line);
  if (arg_count == 0)
    return 0;

  args = (char **)malloc(sizeof(char *) * (arg_count + 1));
  if (args == 0)
    return 0;

  i         = 0;
  arg_index = 0;
  while (line[i] != '\0')
  {
    size_t tmp = count_whitespace(line + i);
    size_t word_len;

    i += tmp;
    if (tmp > 0)
      continue;

    word_len = count_word_without_quotes(line + i) + count_word_with_quotes(line + i);

    tmp_str = cpy_arg_without_quotes(line + i);
    if (tmp_str == 0)
      tmp_str = cpy_arg_with_quotes(line + i);

    args[arg_index] = tmp_str;

    i += word_len;
    arg_index++;
  }
  args[arg_count] = 0;
  return args;
}

void free_args(char **args)
{
  size_t i = 0;

  if (args == 0)
    return;
  while (args[i] != 0)
  {
    free(args[i]);
    i++;
  }
  free(args);
}