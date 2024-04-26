#ifndef INC_PID_ARRAY
#define INC_PID_ARRAY

#include <sys/types.h>

typedef struct pid_array_s {
  pid_t *array;
  size_t size;
  size_t capacity;
} pid_array_t;

pid_array_t pid_array_create(size_t capacity);
void        pid_array_destroy(pid_array_t array);
int         pid_array_push(pid_array_t *array, pid_t pid);
void        pid_array_map(pid_array_t array, void (*callback)(size_t, pid_t));

#endif /* INC_PID_ARRAY */
