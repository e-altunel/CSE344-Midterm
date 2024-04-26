#ifndef INC_MESSAGE
#define INC_MESSAGE

#include <defines.h>
#include <sys/types.h>

int send_data(int fd, void *data, size_t size);
int send_message(int fd, char *message);
int send_int(int fd, int value);

void *receive_data(int fd, size_t *size);
char *receive_message(int fd);
int   receive_int(int fd);

#endif /* INC_MESSAGE */
