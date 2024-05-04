#ifndef INC_PID_ARRAY
#define INC_PID_ARRAY

#include <sys/types.h>

typedef struct {
  pid_t *array;
  size_t size;
  size_t capacity;
} pid_array_t;

pid_array_t *pid_array_create(size_t capacity);
void         pid_array_destroy(pid_array_t *pid_array);
int          pid_array_is_full(pid_array_t *pid_array);
int          pid_array_add(pid_array_t *pid_array, pid_t pid);
int          pid_array_remove(pid_array_t *pid_array, pid_t pid);

#endif /* INC_PID_ARRAY */
