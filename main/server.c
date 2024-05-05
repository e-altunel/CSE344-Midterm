#include <errno.h>
#include <fifo_helper.h>
#include <pid_array.h>
#include <server_helper.h>
#include <signal_helper.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int quit = 0;

static void sigint_handler(int signal) {
  (void)signal;
  quit = 1;
}

static void sigusr2_handler(int signal) {
  (void)signal;
}

int main(int argc, char *argv[]) {
  int          server_fd  = 0;
  ssize_t      bytes_read = 0;
  pid_t        client_pid = 0;
  int          capacity;
  pid_array_t *pid_array;
  int          isParent = 1;
  int          ret      = 0;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <capacity>\n", argv[0]);
    return 1;
  }
  capacity = atoi(argv[1]);
  if (capacity <= 0) {
    fprintf(stderr, "Capacity must be a number greater than 0\n");
    return 1;
  }

  if (handle_signal(SIGINT, sigint_handler) == -1) {
    perror("Failed to set signal handler");
    return 1;
  }
  if (handle_signal(SIGUSR2, sigusr2_handler) == -1) {
    perror("Failed to set signal handler");
    return 1;
  }

  server_fd = create_server_queue();
  if (server_fd == -1) {
    perror("Failed to create server queue");
    return 1;
  }

  pid_array = pid_array_create(capacity);
  if (pid_array == 0) {
    perror("Failed to create pid array");
    return 1;
  }
  printf("Server started with pid: %d\n", getpid());
  while (!quit && isParent) {
    bytes_read = read(server_fd, &client_pid, sizeof(client_pid));
    if (bytes_read == -1) {
      if (errno == EAGAIN) {
        sleep(1);
        continue;
      }
      break;
    }
    if (bytes_read == 0) {
      pause();
      continue;
    }
    if (pid_array_is_full(pid_array)) {
      printf("Client pid array is full\n");
      kill(client_pid, SIGUSR2);
      continue;
    }
    if (accept_client(client_pid, pid_array, &isParent, "server") == -1) {
      if (isParent) {
        perror("Failed to accept client, client should reconnect");
      } else {
        ret = 1;
        break;
      }
    }
    if (!isParent) {
      break;
    }
    printf("Received client pid: %d\n", client_pid);
    bytes_read = 0;
  }
  pid_array_destroy(pid_array);
  if (isParent && destroy_server_queue() == -1) {
    perror("Failed to destroy server queue");
    return 1;
  }
  return ret;
}