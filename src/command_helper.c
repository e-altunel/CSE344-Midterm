#include <command_helper.h>
#include <string.h>

size_t get_size_t(void **ptr) {
  size_t var = *(size_t *)*ptr;
  *ptr       = (size_t *)*ptr + 1;
  return var;
}

void *get_data(void **ptr, size_t size, void *dest) {
  void *var = *ptr;
  *ptr      = (void *)((char *)*ptr + size);
  memcpy(dest, var, size);
  return var;
}