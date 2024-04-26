#define _POSIX_C_SOURCE 1

#include <defines.h>
#include <errno.h>
#include <fcntl.h>
#include <message.h>
#include <server_request.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

static char  client_fifo_read_path[BUFFER_SIZE]  = {0};
static char  client_fifo_write_path[BUFFER_SIZE] = {0};
static pid_t client_pid                          = 0;
static int   client_fifo_read_fd                 = -1;
static int   client_fifo_write_fd                = -1;

static void handle_signal(int signal) {
  (void)signal;

  printf("Client disconnected with PID %d\n", client_pid);
  kill(getppid(), SIGINT);
  if (client_pid != 0) {
    kill(client_pid, SIGINT);
  }
  if (client_fifo_read_fd != -1) {
    close(client_fifo_read_fd);
  }
  if (client_fifo_write_fd != -1) {
    close(client_fifo_write_fd);
  }
  if (client_fifo_read_path[0] != 0) {
    unlink(client_fifo_read_path);
  }
  if (client_fifo_write_path[0] != 0) {
    unlink(client_fifo_write_path);
  }
  exit(0);
}

void handle_client(server_connect_request_t request) {
  struct sigaction action = {0};

  client_pid        = request.client_pid;
  action.sa_handler = handle_signal;
  action.sa_flags   = 0;

  sigemptyset(&action.sa_mask);
  sigaction(SIGINT, &action, NULL);

  printf("Client connected with PID %d\n", request.client_pid);

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
    int value = -1;
    errno     = 0;

    value = receive_int(client_fifo_read_fd);
    if (value == -1) {
      if (errno != 0) {
        perror("Failed to read from client FIFO");
        break;
      }
    }
    printf("Client: %d\n", value);

    sleep(1);

    if (value % 2 == 0) {
      if (send_message(client_fifo_write_fd, "Even") == -1) {
        perror("Failed to send message");
        break;
      }
      printf("Server: Even\n");
    } else {
      if (send_message(client_fifo_write_fd, "Odd") == -1) {
        perror("Failed to send message");
        break;
      }
      printf("Server: Odd\n");
    }
  }
  if (client_pid != 0) {
    kill(client_pid, SIGINT);
  }
}