#ifndef INC_ARGS
#define INC_ARGS

char  *cpy_arg_without_quotes(const char *src);
char  *cpy_arg_with_quotes(const char *src);
char **parse_args(const char *line);
void   free_args(char **args);

#endif /* INC_ARGS */
