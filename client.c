#define _POSIX_C_SOURCE 1

#include <defines.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static char  client_fifo_read_path[BUFFER_SIZE]  = {0};
static char  client_fifo_write_path[BUFFER_SIZE] = {0};
static pid_t server_pid                          = 0;
static int   client_fifo_read_fd                 = -1;
static int   client_fifo_write_fd                = -1;

void handle_signal(int signal) {
  if (server_pid != 0) {
    kill(server_pid, SIGINT);
  }
  if (client_fifo_read_fd != -1) {
    close(client_fifo_read_fd);
  }
  if (client_fifo_write_fd != -1) {
    close(client_fifo_write_fd);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    errno = EINVAL;
    perror("Invalid number of arguments");
    return 1;
  }

  struct sigaction action = {0};

  action.sa_handler = handle_signal;
  action.sa_flags   = 0;
  sigemptyset(&action.sa_mask);
  sigaction(SIGINT, &action, NULL);

  server_pid = atoi(argv[1]);
  if (server_pid == 0) {
    perror("Invalid server PID");
    return 1;
  }

  if (snprintf(client_fifo_read_path, BUFFER_SIZE, "%s%dr", CLIENT_FIFO_PATH, getpid()) < 0) {
    perror("Failed to create client FIFO read path");
    return 1;
  }

  if (snprintf(client_fifo_write_path, BUFFER_SIZE, "%s%dw", CLIENT_FIFO_PATH, getpid()) < 0) {
    perror("Failed to create client FIFO write path");
    return 1;
  }

  if ((client_fifo_read_fd = open(client_fifo_write_path, O_RDONLY)) == -1) {
    perror("Failed to open client FIFO read");
    return 1;
  }

  if ((client_fifo_write_fd = open(client_fifo_read_path, O_WRONLY)) == -1) {
    perror("Failed to open client FIFO write");
    return 1;
  }

  while (1) {
    char buffer[BUFFER_SIZE]  = {0};
    char message[BUFFER_SIZE] = "Hello, server!";
    int  return_value;

    if (write(client_fifo_read_fd, message, BUFFER_SIZE) == -1) {
      perror("Failed to write to stdout");
      break;
    }

    return_value = read(client_fifo_write_fd, buffer, BUFFER_SIZE);
    if (return_value == -1) {
      perror("Failed to read from stdin");
      break;
    }
  }
  close(client_fifo_read_fd);
  close(client_fifo_write_fd);

  return 0;
}