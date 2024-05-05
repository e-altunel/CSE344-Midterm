#ifndef INC_COMMAND_HELPER
#define INC_COMMAND_HELPER

#include <stddef.h>

size_t get_size_t(void **ptr);
void  *get_data(void **ptr, size_t size, void *dest);

#endif /* INC_COMMAND_HELPER */
