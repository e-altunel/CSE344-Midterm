#include <errno.h>
#include <message.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>

static void enter_collection(int recv) {
  message_t *message = 0;

  while (1) {
    message = message_recv(recv);
    if (message == 0)
      break;
    if (message->type == MSG_TYPE_END_COLLECTION || message->type == MSG_TYPE_ERROR || message->type == MSG_TYPE_QUIT)
      break;
    message_print_data(message);
    message_destroy(message);
  }
  if (message != 0)
    message_destroy(message);
}

void handle_message_client(const message_t *message, int send, int recv) {
  int *tmp;

  switch (message->type) {
  case MSG_TYPE_ERROR:
  case MSG_TYPE_STRING:
  case MSG_TYPE_END_COLLECTION:
  case MSG_TYPE_QUIT:
    message_print(message);
    break;
  case MSG_TYPE_PING:
    sleep(1);
    tmp = (int *)message->data;
    send_ping(send, *tmp + 1);
    break;
  case MSG_TYPE_COLLECTION:
    enter_collection(recv);
    break;
  default:
    send_error(send, "Unknown message type");
    break;
  }
}

static void *dump_data(size_t argc, char **argv, size_t *size) {
  size_t i;
  size_t total_size = 0;
  char  *data       = 0;

  for (i = 0; i < argc; i++) {
    total_size += strlen(argv[i]) + 1;
  }
  total_size += argc * sizeof(size_t) + sizeof(size_t);
  data = malloc(total_size);
  if (data == 0) {
    return 0;
  }
  *size = total_size;
  i     = 0;
  memcpy(data, &argc, sizeof(size_t));
  while (i < argc) {
    memcpy(data, &total_size, sizeof(size_t));
    data += sizeof(size_t);
    memcpy(data, argv[i], strlen(argv[i]) + 1);
    data += strlen(argv[i]) + 1;
    i++;
  }
  return data - total_size;
}

void send_command_with_data(int argc, char **argv, int send) {
  size_t    size = 0;
  command_t command;
  void     *data;

  if (argc < 1) {
    errno = EINVAL;
    return;
  }
  data = dump_data(argc, argv, &size);
  if (data == 0) {
    errno = ENOMEM;
    return;
  }

  command.size = size;
  command.data = data;

  if (!strcmp(argv[0], "help"))
    command.type = CMD_TYPE_HELP;
  else if (!strcmp(argv[0], "list"))
    command.type = CMD_TYPE_LIST;
  else if (!strcmp(argv[0], "readF"))
    command.type = CMD_TYPE_READF;
  else if (!strcmp(argv[0], "writeT"))
    command.type = CMD_TYPE_WRITET;
  else if (!strcmp(argv[0], "upload"))
    command.type = CMD_TYPE_UPLOAD;
  else if (!strcmp(argv[0], "download"))
    command.type = CMD_TYPE_DOWNLOAD;
  else if (!strcmp(argv[0], "archive"))
    command.type = CMD_TYPE_ARCHIVE;
  else if (!strcmp(argv[0], "kill"))
    command.type = CMD_TYPE_KILL;
  else if (!strcmp(argv[0], "quit"))
    command.type = CMD_TYPE_QUIT;
  else {
    free(data);
    errno = EINVAL;
    return;
  }

  send_command(send, &command);
  free(data);
}