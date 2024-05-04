#include <errno.h>
#include <fifo_helper.h>
#include <server_helper.h>
#include <signal.h>
#include <signal_helper.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static volatile int quit = 0;

static void sigint_handler(int signal) {
  (void)signal;
  quit = 1;
}

int accept_client(pid_t client_pid, pid_array_t *pid_array, int *isParent) {
  pid_t child_pid;
  pid_t my_pid = getpid();
  int   recv   = 0;
  int   send   = 0;
  int   ret;

  if (pid_array == 0 || isParent == 0) {
    errno = EINVAL;
    return -1;
  }

  child_pid = fork();
  if (child_pid == -1)
    return -1;
  if (child_pid != 0) {
    *isParent = 1;
    if (pid_array_add(pid_array, child_pid) == -1) {
      kill(child_pid, SIGINT);
      return -1;
    }
    return 0;
  }
  *isParent = 0;
  if (handle_signal(SIGINT, sigint_handler) == -1) {
    return -1;
  }

  kill(client_pid, SIGUSR1);
  recv = create_process_comm(client_pid, my_pid, O_RDONLY);
  if (recv == -1)
    return -1;
  send = create_process_comm(my_pid, client_pid, O_WRONLY);
  if (send == -1) {
    close(recv);
    destroy_process_comm(client_pid, my_pid);
    return -1;
  }
  ret = 0;
  if (child_job(client_pid) == -1)
    ret = -1;

  close(recv);
  close(send);
  destroy_process_comm(client_pid, my_pid);
  destroy_process_comm(my_pid, client_pid);

  return ret;
}

int child_job(pid_t client_pid) {
  kill(client_pid, SIGINT);
  return 0;
}