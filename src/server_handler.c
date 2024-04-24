#define _POSIX_C_SOURCE 1

#include <defines.h>
#include <errno.h>
#include <fcntl.h>
#include <server_request.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

static char  client_fifo_read_path[BUFFER_SIZE]  = {0};
static char  client_fifo_write_path[BUFFER_SIZE] = {0};
static pid_t client_pid                          = 0;
static int   client_fifo_read_fd                 = -1;
static int   client_fifo_write_fd                = -1;

static void handle_signal(int signal) {
  (void)signal;

  kill(getppid(), SIGINT);
  if (client_fifo_read_path[0] != 0) {
    unlink(client_fifo_read_path);
  }
  if (client_fifo_write_path[0] != 0) {
    unlink(client_fifo_write_path);
  }
  if (client_pid != 0) {
    kill(client_pid, SIGINT);
  }
  if (client_fifo_read_fd != -1) {
    close(client_fifo_read_fd);
  }
  if (client_fifo_write_fd != -1) {
    close(client_fifo_write_fd);
  }
}

void handle_client(server_connect_request_t request) {
  struct sigaction action = {0};

  client_pid        = request.client_pid;
  action.sa_handler = handle_signal;
  action.sa_flags   = 0;

  sigemptyset(&action.sa_mask);
  sigaction(SIGINT, &action, NULL);

  if (snprintf(client_fifo_read_path, BUFFER_SIZE, "%s%dr", CLIENT_FIFO_PATH, request.client_pid) < 0) {
    perror("Failed to create client FIFO read path");
    return;
  }
  if (snprintf(client_fifo_write_path, BUFFER_SIZE, "%s%dw", CLIENT_FIFO_PATH, request.client_pid) < 0) {
    perror("Failed to create client FIFO write path");
    return;
  }

  if (mkfifo(client_fifo_read_path, 0666) == -1 && errno != EEXIST) {
    perror("Failed to create client FIFO read");
    return;
  }

  if (mkfifo(client_fifo_write_path, 0666) == -1 && errno != EEXIST) {
    unlink(client_fifo_read_path);
    perror("Failed to create client FIFO write");
    return;
  }

  if ((client_fifo_read_fd = open(client_fifo_read_path, O_RDONLY)) == -1) {
    perror("Failed to open client FIFO read");
    unlink(client_fifo_read_path);
    unlink(client_fifo_write_path);
    return;
  }

  if ((client_fifo_write_fd = open(client_fifo_write_path, O_WRONLY)) == -1) {
    perror("Failed to open client FIFO write");
    close(client_fifo_read_fd);
    unlink(client_fifo_read_path);
    unlink(client_fifo_write_path);
    return;
  }

  while (1) {
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE] = "Hello, client!";
    int  return_value;

    return_value = read(client_fifo_read_fd, buffer, BUFFER_SIZE);
    if (return_value == -1) {
      perror("Failed to read from client FIFO");
      break;
    }
    if (return_value == 0) {
      break;
    }

    if (write(client_fifo_write_fd, message, sizeof(message)) == -1) {
      perror("Failed to write to client FIFO");
      break;
    }
  }
}