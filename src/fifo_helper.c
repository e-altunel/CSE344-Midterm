#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <errno.h>
#include <fcntl.h>
#include <fifo_helper.h>
#include <signal.h>
#include <signal_helper.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int create_server_queue() {
  char buffer[BUFFER_SIZE];
  int  fd;
  int  tmp;

  tmp = snprintf(buffer, BUFFER_SIZE, GLOBAL_SERVER_QUEUE_FORMAT, getpid());
  if (tmp < 0 || tmp >= BUFFER_SIZE) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if (mkfifo(buffer, 0666) == -1)
    return -1;

  fd = open(buffer, O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    unlink(buffer);
    return -1;
  }

  return fd;
}

int destroy_server_queue() {
  char buffer[BUFFER_SIZE];
  int  tmp;

  tmp = snprintf(buffer, BUFFER_SIZE, GLOBAL_SERVER_QUEUE_FORMAT, getpid());
  if (tmp < 0 || tmp >= BUFFER_SIZE) {
    errno = ENAMETOOLONG;
    return -1;
  }

  return unlink(buffer);
}

int connect_server(pid_t server_pid, int flags, int *recv, int *send) {
  int tmp;

  if (recv == 0 || send == 0) {
    errno = EINVAL;
    return -1;
  }

  if (connect_server_queue(server_pid, flags) == -1)
    return -1;

  tmp = connect_process_comm(getpid(), server_pid, O_WRONLY);
  if (tmp == -1) {
    kill(server_pid, SIGUSR2);
    return -1;
  }
  *send = tmp;

  tmp = connect_process_comm(server_pid, getpid(), O_RDONLY);
  if (tmp == -1) {
    kill(server_pid, SIGUSR2);
    return -1;
  }
  *recv = tmp;

  return 0;
}

static int server_answer = 0;

static pid_t server_answer_pid = 0;

static void server_answer_handler(int singal, siginfo_t *info, void *context) {
  (void)context;
  (void)info;
  if (singal == SIGUSR1)
    server_answer = 1;
  if (singal == SIGUSR2)
    server_answer = 2;
  server_answer_pid = info->si_pid;
}

pid_t connect_server_queue(pid_t server_pid, int flags) {
  char  buffer[BUFFER_SIZE];
  pid_t own_pid;
  int   fd;
  int   tmp;

  if (handle_signal_detail(SIGUSR1, server_answer_handler, 0) == -1 ||
      handle_signal_detail(SIGUSR2, server_answer_handler, 0) == -1) {
    return -1;
  }

  while (1) {
    tmp = snprintf(buffer, BUFFER_SIZE, GLOBAL_SERVER_QUEUE_FORMAT, server_pid);
    if (tmp < 0 || tmp >= BUFFER_SIZE) {
      errno = ENAMETOOLONG;
      return -1;
    }

    fd = open(buffer, O_WRONLY);
    if (fd == -1) {
      sigaction(SIGUSR1, 0, 0);
      sigaction(SIGUSR2, 0, 0);
      return -1;
    }

    own_pid = getpid();
    if (write(fd, &own_pid, sizeof(pid_t)) == -1) {
      close(fd);
      sigaction(SIGUSR1, 0, 0);
      sigaction(SIGUSR2, 0, 0);
      return -1;
    }
    kill(server_pid, SIGUSR2);
    close(fd);

    while (server_answer == 0)
      pause();

    if (server_answer == 2 && !(flags & O_NONBLOCK)) {
      errno = ECONNREFUSED;

      break;
    }
    if (server_answer == 1) {
      server_answer = 0;
      return server_answer_pid;
    }
    server_answer = 0;
    printf("Server busy, retrying in 1 second\n");
    sleep(1);
  }
  sigaction(SIGUSR1, 0, 0);
  sigaction(SIGUSR2, 0, 0);
  return -1;
}

int connect_process_comm(pid_t from_pid, pid_t to_pid, int flags) {
  char buffer[BUFFER_SIZE];
  int  fd;
  int  tmp;

  tmp = snprintf(buffer, BUFFER_SIZE, PROCESS_COMM_FORMAT, from_pid, to_pid);
  if (tmp < 0 || tmp >= BUFFER_SIZE) {
    errno = ENAMETOOLONG;
    return -1;
  }

  fd = open(buffer, flags);
  if (fd == -1)
    return -1;

  return fd;
}

int create_process_comm(pid_t from_pid, pid_t to_pid, int flags) {
  char buffer[BUFFER_SIZE];
  int  tmp;

  printf("Creating process comm %d -> %d\n", from_pid, to_pid);
  tmp = snprintf(buffer, BUFFER_SIZE, PROCESS_COMM_FORMAT, from_pid, to_pid);
  if (tmp < 0 || tmp >= BUFFER_SIZE) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if (mkfifo(buffer, 0666) == -1)
    return -1;

  tmp = open(buffer, flags);
  if (tmp == -1) {
    unlink(buffer);
    return -1;
  }
  return tmp;
}

int destroy_process_comm(pid_t from_pid, pid_t to_pid) {
  char buffer[BUFFER_SIZE];
  int  tmp;

  tmp = snprintf(buffer, BUFFER_SIZE, PROCESS_COMM_FORMAT, from_pid, to_pid);
  if (tmp < 0 || tmp >= BUFFER_SIZE) {
    errno = ENAMETOOLONG;
    return -1;
  }

  return unlink(buffer);
}