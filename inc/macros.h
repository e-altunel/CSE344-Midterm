#ifndef INC_MACROS
#define INC_MACROS

#ifndef NDEBUG
#define ASSERT_GOTO(condition, label)                                                                                  \
  if (!(condition))                                                                                                    \
  {                                                                                                                    \
    fprintf(stderr, "Assertion failed: %d in %s:%d\n", __LINE__, __FILE__, #condition);                                \
    goto label;                                                                                                        \
  }
#else
#define ASSERT_GOTO(condition, label)                                                                                  \
  if (!(condition))                                                                                                    \
  {                                                                                                                    \
    goto label;                                                                                                        \
  }
#endif

#endif /* INC_MACROS */
