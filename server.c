#define _POSIX_C_SOURCE 1

#include <defines.h>
#include <errno.h>
#include <fcntl.h>
#include <pid_array.h>
#include <server_handler.h>
#include <server_request.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

void print_pid(size_t index, pid_t pid) {
  printf("PID at index %ld: %d\n", index, pid);
}

int main() {
  pid_array_t pids = pid_array_create(10);
  int         server_fifo_fd;
  time_t      last_read;

  if (mkfifo(SERVER_FIFO_PATH, 0666) == -1 && errno != EEXIST) {
    perror("Failed to create server FIFO");
    return 1;
  }

  if ((server_fifo_fd = open(SERVER_FIFO_PATH, O_RDONLY | O_NONBLOCK)) == -1) {
    perror("Failed to open server FIFO");
    return 1;
  }

  last_read = time(NULL);
  while (1) {
    server_connect_request_t request = {0};
    int                      return_value;
    pid_t                    child_pid;

    return_value = read(server_fifo_fd, &request, sizeof(request));

    if (return_value == -1 || return_value == 0) {
      if (time(NULL) - last_read > 3) {
        errno = ETIMEDOUT;
        perror("Server waited for too long");
        break;
      }
      continue;
    }
    last_read = time(NULL);

    child_pid = fork();
    if (child_pid == -1) {
      perror("Failed to fork");
      break;
    }

    if (child_pid == 0) {
      close(server_fifo_fd);
      pid_array_destroy(pids);
      handle_client(request);
      return 0;
    }
  }
  close(server_fifo_fd);
  pid_array_destroy(pids);
  unlink(SERVER_FIFO_PATH);

  return 0;
}
