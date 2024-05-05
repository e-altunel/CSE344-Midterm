#include <client_helper.h>
#include <fifo_helper.h>
#include <message.h>
#include <signal_helper.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static volatile int quit = 0;

static void sigint_handler(int signal) {
  (void)signal;
  quit = 1;
}

int main(int argc, char *argv[]) {
  int        server_pid = 0;
  int        recv       = 0;
  int        send       = 0;
  message_t *message;
  command_t  command = {0};
  char       tmp[]   = "help";

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <server_pid>\n", argv[0]);
    return 1;
  }

  if (handle_signal(SIGINT, sigint_handler) == -1 || handle_signal(SIGPIPE, sigint_handler) == -1) {
    perror("Failed to set signal handler");
    return 1;
  }

  server_pid = atoi(argv[1]);
  if (connect_server(server_pid, 0, &recv, &send) == -1) {
    perror("Failed to connect to server");
    return 1;
  }

  printf("Connected to server %d\n", server_pid);

  command.type = CMD_TYPE_HELP;
  command.data = tmp;
  command.size = sizeof(tmp);
  send_command(send, &command);

  while (!quit) {
    message = message_recv(recv);
    if (message == 0) {
      kill(server_pid, SIGINT);
      break;
    }
    if (message->type == MSG_TYPE_QUIT)
      quit = 1;
    handle_message_client(message, send, recv);
    message_destroy(message);
    break;
  }
  send_quit(send);

  close(recv);
  close(send);

  printf("Disconnected from server %d\n", server_pid);

  return 0;
}