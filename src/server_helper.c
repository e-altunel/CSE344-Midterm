#include <errno.h>
#include <fifo_helper.h>
#include <message.h>
#include <server_helper.h>
#include <signal.h>
#include <signal_helper.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static volatile int quit = 0;

static void sigint_handler(int signal) {
  (void)signal;
  quit = 1;
}

int accept_client(pid_t client_pid, pid_array_t *pid_array, int *isParent, const char *dirname) {
  pid_t child_pid;
  pid_t my_pid;
  int   recv = 0;
  int   send = 0;
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

  my_pid = getpid();
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
  kill(client_pid, SIGUSR1);
  ret = 0;
  if (child_job(client_pid, recv, send, dirname) == -1)
    ret = -2;

  close(recv);
  close(send);
  destroy_process_comm(client_pid, my_pid);
  destroy_process_comm(my_pid, client_pid);

  printf("Client %d disconnected\n", client_pid);

  return ret;
}

int child_job(pid_t client_pid, int recv, int send, const char *dirname) {
  message_t *message;
  (void)send;

  quit = 0;
  if (handle_signal(SIGPIPE, sigint_handler) == -1) {
    return -1;
  }

  while (!quit) {
    message = message_recv(recv);
    if (message == 0) {
      kill(client_pid, SIGINT);
      return -1;
    }
    message_print(message);
    if (message->type == MSG_TYPE_QUIT)
      quit = 1;

    handle_message_server(message, dirname, send);
    message_destroy(message);
  }
  send_quit(send);

  return 0;
}

void handle_message_server(const message_t *message, const char *dirname, int send) {
  int *tmp;

  switch (message->type) {
  case MSG_TYPE_QUIT:
    quit = 1;
    break;
  case MSG_TYPE_PING:
    sleep(1);
    tmp = (int *)message->data;
    send_ping(send, *tmp + 1);
    break;
  case MSG_TYPE_COMMAND:
    handle_command_server(message->data, dirname, send);
    break;
  default:
    send_error(send, "Unknown message type");
    break;
  }
}

void handle_command_server(void *command_dat, const char *dirname, int send) {
  command_t command;
  void     *ptr;

  if (command_dat == 0) {
    send_error(send, "Invalid command");
    errno = EINVAL;
    return;
  }

  command.type = *(command_type_t *)command_dat;
  ptr          = (char *)command_dat + sizeof(command_type_t);
  printf("Command: %d\n", command.type);
  switch (command.type) {
  case CMD_TYPE_HELP:
    send_help(send, ptr);
    break;
  case CMD_TYPE_LIST:
    send_list(send, dirname);
    break;
  case CMD_TYPE_READF:
    send_readf(send, ptr);
    break;
  case CMD_TYPE_WRITET:
    send_writet(send, ptr);
    break;
  case CMD_TYPE_UPLOAD:
    send_upload(send, ptr);
    break;
  case CMD_TYPE_DOWNLOAD:
    send_download(send, ptr);
    break;
  case CMD_TYPE_ARCHIVE:
    send_archive(send, ptr);
    break;
  case CMD_TYPE_KILL:
    send_kill(send);
    break;
  case CMD_TYPE_QUIT:
    send_quit(send);
    quit = 1;
    break;
  }
}