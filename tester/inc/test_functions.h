#ifndef TESTER_INC_TEST_FUNCTIONS
#define TESTER_INC_TEST_FUNCTIONS

char  *str_cpy(const char *src);
char **str_array_cpy(const char **src);

int  str_not_equal(const char *x, const char *y);
int  str_array_not_equal(const char **x, const char **y);
void safe_free(void *ptr);

#endif /* TESTER_INC_TEST_FUNCTIONS */
