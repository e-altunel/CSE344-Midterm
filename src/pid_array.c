#include <defines.h>
#include <errno.h>
#include <pid_array.h>
#include <stdlib.h>

pid_array_t pid_array_create(size_t size) {
  pid_array_t array = {0};

  if (size == 0 || size > MAX_CLIENTS) {
    errno = EINVAL;
    return array;
  }

  array.array = calloc(size, sizeof(pid_t));
  if (array.array == 0)
    return array;

  array.size     = 0;
  array.capacity = size;

  return array;
}

void pid_array_destroy(pid_array_t array) {
  if (array.array != 0 && array.capacity > 0) {
    free(array.array);
    array.array    = 0;
    array.size     = 0;
    array.capacity = 0;
    return;
  }
  errno = EINVAL;
}

int pid_array_push(pid_array_t *array, pid_t pid) {
  if (array == 0 || array->array == 0) {
    errno = EINVAL;
    return -1;
  }
  if (array->capacity == 0 || array->size >= array->capacity) {
    errno = ENOMEM;
    return -1;
  }

  array->array[array->size++] = pid;
  return 0;
}

void pid_array_map(pid_array_t array, void (*callback)(size_t, pid_t)) {
  size_t i = 0;

  if (array.array == 0) {
    errno = EINVAL;
    return;
  }

  while (i < array.size) {
    callback(i, array.array[i]);
    i++;
  }
}