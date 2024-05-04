#include <errno.h>
#include <sys/types.h>

/**
 * @brief Check if a character is a whitespace character. Whitespace characters are defined as ' ', '\t', and '\n'.
 *
 * @param c character to check
 * @return int 1 if the character is a whitespace character, 0 otherwise
 */
int isWhitespace(char c)
{
  return c == ' ' || c == '\t' || c == '\n';
}

/**
 * @brief Count the number of characters in a word. Words are defined as a sequence of characters that are not
 * whitespace. If the first character is a whitespace, then the function will return 0. If there is a quote, then the
 * function returns 0 and sets errno to EINVAL.
 *
 * @param ptr NULL terminated string
 * @return size_t number of characters in the word
 */
size_t count_word_without_quotes(const char *ptr)
{
  size_t count = 0;

  while (ptr[count] != '\0' && !isWhitespace(ptr[count]) && ptr[count] != '"')
    count++;

  if (ptr[count] == '"')
  {
    errno = EINVAL;
    return 0;
  }

  return count;
}

/**
 * @brief Count the number of characters in a word. Words are defined as a sequence of characters that are not
 * whitespace and are enclosed in quotes. If the first character is not a quote, then the function will return 0 and set
 * errno to EINVAL. Next character after the second quote should be a whitespace or a NULL character. If it is not, then
 * the function will return 0 and set errno to EINVAL.
 *
 * @example "hello world" -> 11
 * @example "hello world -> 0
 * @example hello world" -> 0
 * @example hello world -> 0
 * @example "hello world"a -> 0
 *
 * @param ptr NULL terminated string
 * @return size_t number of characters in the word with quotes along with the quotes count
 *
 * @brief invalid input will return 0 and set errno to EINVAL
 */
size_t count_word_with_quotes(const char *ptr)
{
  size_t count = 0;

  if (ptr[count] != '"')
  {
    errno = EINVAL;
    return 0;
  }
  count++;
  while (ptr[count] != '\0' && ptr[count] != '"')
    count++;

  if (ptr[count] == '\0')
  {
    errno = EINVAL;
    return 0;
  }

  if (!isWhitespace(ptr[count + 1]) && ptr[count + 1] != '\0')
  {
    errno = EINVAL;
    return 0;
  }

  return count + 1;
}

/**
 * @brief Count the number of whitespace characters in a string.
 *
 * @param ptr NULL terminated string
 * @return size_t number of whitespace characters
 */
size_t count_whitespace(const char *ptr)
{
  size_t count = 0;

  while (isWhitespace(ptr[count]))
    count++;

  return count;
}

/**
 * @brief Count the number of arguments in a string. Arguments are defined as a sequence of characters that are not
 * whitespace. If the first character is a whitespace, then the function will return 0. If there is a quote, then the
 * function returns 0 and sets errno to EINVAL.
 *
 * @param line NULL terminated string
 * @return size_t number of arguments
 */
size_t count_args(const char *line)
{
  size_t word_count = 0;
  size_t i          = 0;
  size_t tmp;

  errno = 0;
  while (line[i] != '\0')
  {
    tmp = count_whitespace(line + i);
    i += tmp;
    if (tmp > 0)
      continue;
    tmp = count_word_without_quotes(line + i) + count_word_with_quotes(line + i);
    i += tmp;
    word_count += tmp > 0;

    if (tmp == 0 && errno == EINVAL)
      return 0;
  }

  return word_count;
}
