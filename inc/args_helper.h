#ifndef INC_ARGS_HELPER
#define INC_ARGS_HELPER

#include <sys/types.h>

size_t count_args(const char *line);
size_t count_whitespace(const char *ptr);
size_t count_word_without_quotes(const char *ptr);
size_t count_word_with_quotes(const char *ptr);
int    isWhitespace(char c);

#endif /* INC_ARGS_HELPER */
