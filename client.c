#define _POSIX_C_SOURCE 200809L

#include <defines.h>
#include <errno.h>
#include <fcntl.h>
#include <message.h>
#include <server_request.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static pid_t server_pid           = 0;
static int   client_fifo_read_fd  = -1;
static int   client_fifo_write_fd = -1;
static int   server_fifo_fd       = -1;
static char *buffer               = 0;

void handle_signal(int signal) {
  (void)signal;

  if (server_pid != 0) {
    kill(server_pid, SIGINT);
  }
  if (client_fifo_read_fd != -1) {
    close(client_fifo_read_fd);
  }
  if (client_fifo_write_fd != -1) {
    close(client_fifo_write_fd);
  }
  if (server_fifo_fd != -1) {
    close(server_fifo_fd);
  }
  if (buffer != 0) {
    free(buffer);
  }
  exit(0);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    errno = EINVAL;
    perror("Invalid number of arguments");
    return 1;
  }

  char                     client_fifo_read_path[BUFFER_SIZE]  = {0};
  char                     client_fifo_write_path[BUFFER_SIZE] = {0};
  struct sigaction         action                              = {0};
  server_connect_request_t request;

  action.sa_handler = handle_signal;
  action.sa_flags   = 0;
  sigemptyset(&action.sa_mask);
  sigaction(SIGINT, &action, NULL);

  server_pid = atoi(argv[1]);
  if (server_pid == 0) {
    perror("Invalid server PID");
    return 1;
  }

  if ((server_fifo_fd = open(SERVER_FIFO_PATH, O_WRONLY)) == -1) {
    perror("Failed to open server FIFO");
    return 1;
  }

  request.client_pid = getpid();
  if (write(server_fifo_fd, &request, sizeof(request)) == -1) {
    perror("Failed to write to server FIFO");
    return 1;
  }

  close(server_fifo_fd);

  if (snprintf(client_fifo_read_path, BUFFER_SIZE, "%s%dr", CLIENT_FIFO_PATH, getpid()) < 0) {
    perror("Failed to create client FIFO read path");
    return 1;
  }

  if (snprintf(client_fifo_write_path, BUFFER_SIZE, "%s%dw", CLIENT_FIFO_PATH, getpid()) < 0) {
    perror("Failed to create client FIFO write path");
    return 1;
  }

  while (access(client_fifo_read_path, F_OK) == -1 || access(client_fifo_write_path, F_OK) == -1) {
    continue;
  }

  if ((client_fifo_write_fd = open(client_fifo_read_path, O_WRONLY)) == -1) {
    perror("Failed to open client FIFO write");
    return 1;
  }

  if ((client_fifo_read_fd = open(client_fifo_write_path, O_RDONLY)) == -1) {
    perror("Failed to open client FIFO read");
    return 1;
  }

  srand(time(NULL));

  while (1) {
    int value = rand() % 100;

    if (send_int(client_fifo_write_fd, value) == -1) {
      perror("Failed to send message");
      break;
    }
    printf("Client: %d\n", value);

    sleep(1);

    buffer = receive_message(client_fifo_read_fd);
    if (buffer == 0) {
      perror("Failed to read from stdin");
      break;
    }
    printf("Server: %s\n", buffer);
    free(buffer);
    buffer = 0;
  }
  if (buffer != 0) {
    free(buffer);
  }
  if (server_pid != 0) {
    kill(server_pid, SIGINT);
  }
  close(client_fifo_read_fd);
  close(client_fifo_write_fd);

  return 0;
}