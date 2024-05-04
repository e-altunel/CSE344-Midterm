#include <errno.h>
#include <pid_array.h>
#include <stdlib.h>

pid_array_t *pid_array_create(size_t capacity) {
  pid_array_t *pid_array;
  if (capacity == 0) {
    errno = EINVAL;
    return 0;
  }
  pid_array = malloc(sizeof(pid_array_t));
  if (pid_array == 0)
    return 0;

  pid_array->array = calloc(capacity, sizeof(pid_t));
  if (pid_array->array == 0) {
    free(pid_array);
    return 0;
  }

  pid_array->size     = 0;
  pid_array->capacity = capacity;

  return pid_array;
}

void pid_array_destroy(pid_array_t *pid_array) {
  if (pid_array == 0)
    return;
  if (pid_array->array != 0)
    free(pid_array->array);
  free(pid_array);
}

int pid_array_is_full(pid_array_t *pid_array) {
  if (pid_array == 0 || pid_array->array == 0 || pid_array->capacity == 0)
    return -1;
  return pid_array->size == pid_array->capacity;
}

int pid_array_add(pid_array_t *pid_array, pid_t pid) {
  size_t index = 0;

  if (pid_array == 0 || pid == 0 || pid_array->array == 0 || pid_array->capacity == 0) {
    errno = EINVAL;
    return -1;
  }
  if (pid_array->size == pid_array->capacity)
    return -1;
  while (index < pid_array->capacity) {
    if (pid_array->array[index] == 0) {
      pid_array->array[index] = pid;
      return index;
    }
    index++;
  }
  errno = ENOMEM;
  return -1;
}

int pid_array_remove(pid_array_t *pid_array, pid_t pid) {
  size_t index = 0;

  if (pid_array == 0 || pid == 0 || pid_array->array == 0 || pid_array->capacity == 0) {
    errno = EINVAL;
    return -1;
  }
  while (index < pid_array->capacity) {
    if (pid_array->array[index] == pid) {
      pid_array->array[index] = 0;
      return index;
    }
    index++;
  }
  errno = ENOENT;
  return -1;
}
