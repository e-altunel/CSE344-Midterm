#ifndef INC_PID_ARRAY
#define INC_PID_ARRAY

#include <sys/types.h>

typedef struct pid_array_s {
  pid_t *array;
  size_t size;
  size_t capacity;
} pid_array_t;

pid_array_t pid_array_create(size_t);
void        pid_array_destroy(pid_array_t);
int         pid_array_push(pid_array_t *, pid_t);
void        pid_array_map(pid_array_t, void (*callback)(size_t, pid_t));

#endif /* INC_PID_ARRAY */
