#include <defines.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int send_data(int fd, void *data, size_t size) {
  if (write(fd, &size, sizeof(size_t)) == -1) {
    return -1;
  }
  return write(fd, data, size);
}

int send_message(int fd, char *message) {
  return send_data(fd, message, strlen(message));
}

int send_int(int fd, int value) {
  return send_data(fd, &value, sizeof(int));
}

void *receive_data(int fd, size_t *size) {
  size_t data_size = 0;
  if (read(fd, &data_size, sizeof(size_t)) == -1) {
    return 0;
  }
  void *data = calloc(data_size + 1, sizeof(char));
  if (data == 0) {
    return 0;
  }
  if (read(fd, data, data_size) == -1) {
    free(data);
    return 0;
  }
  if (size != 0) {
    *size = data_size;
  }
  return data;
}

char *receive_message(int fd) {
  return receive_data(fd, 0);
}

int receive_int(int fd) {
  int    value = 0;
  size_t size  = 0;
  errno        = 0;

  if (read(fd, &size, sizeof(size_t)) == -1) {
    return -1;
  }
  if (size != sizeof(int)) {
    errno = EINVAL;
    return -1;
  }
  if (read(fd, &value, sizeof(int)) == -1) {
    return -1;
  }
  return value;
}