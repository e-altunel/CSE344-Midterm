#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <errno.h>
#include <file_helper.h>
#include <message.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

message_t *message_create(message_type_t type, size_t size) {
  message_t *message = malloc(sizeof(message_t));
  if (message == 0)
    return 0;

  message->type      = type;
  message->timestamp = time(0);
  message->size      = size;
  message->data      = 0;

  if (size > 0)
    message->data = malloc(size);

  if (size > 0 && message->data == 0) {
    free(message);
    return 0;
  }

  return message;
}

message_t *message_create_string(const char *string) {
  message_t *message;
  size_t     size;

  if (string == 0)
    return 0;

  size    = strlen(string) + 1;
  message = message_create(MSG_TYPE_STRING, size);
  if (message == 0)
    return 0;

  memcpy(message->data, string, size);

  return message;
}

void message_destroy(message_t *message) {
  if (message == 0)
    return;
  if (message->data != 0)
    free(message->data);
  free(message);
}

int message_send(int fd, const message_t *message) {
  if (message == 0 || fd < 0 || (message->size > 0 && message->data == 0)) {
    errno = EINVAL;
    return -1;
  }

  if (write(fd, &message->type, sizeof(message->type)) == -1)
    return -1;
  if (write(fd, &message->timestamp, sizeof(message->timestamp)) == -1)
    return -1;
  if (write(fd, &message->size, sizeof(message->size)) == -1)
    return -1;
  if (message->size > 0 && write(fd, message->data, message->size) == -1)
    return -1;

  return 0;
}

message_t *message_recv(int fd) {
  message_t *message;

  if (fd < 0) {
    errno = EINVAL;
    return 0;
  }

  message = calloc(1, sizeof(message_t));
  if (message == 0)
    return 0;

  if (read(fd, &message->type, sizeof(message->type)) == -1) {
    free(message);
    return 0;
  }
  if (read(fd, &message->timestamp, sizeof(message->timestamp)) == -1) {
    free(message);
    return 0;
  }
  if (read(fd, &message->size, sizeof(message->size)) == -1) {
    free(message);
    return 0;
  }
  if (message->size > 0) {
    message->data = malloc(message->size);
    if (message->data == 0) {
      free(message);
      return 0;
    }
    if (read(fd, message->data, message->size) == -1) {
      free(message->data);
      free(message);
      return 0;
    }
  } else {
    message->data = 0;
  }

  return message;
}

void message_print(const message_t *message) {
  if (message == 0) {
    printf("None\n");
    return;
  }

  time_print(message->timestamp);
  printf(" Message type: ");
  switch (message->type) {
  case MSG_TYPE_QUIT:
    printf("Quit\n");
    break;
  case MSG_TYPE_PING:
    printf("Ping %d\n", *(int *)message->data);
    break;
  case MSG_TYPE_STRING:
    printf("String\n");
    if (message->size > 0 && message->data != 0)
      printf("%s\n", (char *)message->data);
    break;
  case MSG_TYPE_ERROR:
    printf("Error\n");
    if (message->size > 0 && message->data != 0)
      printf("%s\n", (char *)message->data);
    break;
  case MSG_TYPE_COMMAND:
    printf("Command\n");
    break;
  default:
    printf("Unknown (%d)\n", message->type);
    break;
  }
}

void message_print_data(const message_t *message) {
  if (message == 0 || message->size == 0 || message->data == 0)
    return;

  switch (message->type) {
  case MSG_TYPE_STRING:
    printf("%s\n", (char *)message->data);
    break;
  default:
    break;
  }
}

void time_print(const time_t time) {
  printf("[%lu]", time);
}

int send_command(int fd, const command_t *command) {
  message_t *message;

  if (command == 0) {
    errno = EINVAL;
    return -1;
  }

  message = message_create(MSG_TYPE_COMMAND, command->size + sizeof(command->type) + sizeof(command->size));
  if (message == 0)
    return -1;
  message->size = command->size + sizeof(command->type) + sizeof(command->size);
  memcpy(message->data, &command->type, sizeof(command->type));
  memcpy((char *)message->data + sizeof(command->type), &command->size, sizeof(command->size));
  if (command->size > 0) {
    memcpy((char *)message->data + sizeof(command->type) + sizeof(command->size), command->data, command->size);
  }

  if (message_send(fd, message) == -1) {
    message_destroy(message);
    return -1;
  }

  message_destroy(message);
  return 0;
}

int send_string(int fd, const char *string) {
  message_t *message;

  if (string == 0)
    return -1;

  message = message_create_string(string);
  if (message == 0)
    return -1;

  if (message_send(fd, message) == -1) {
    message_destroy(message);
    return -1;
  }

  message_destroy(message);
  return 0;
}

int send_error(int fd, const char *error) {
  message_t *message;

  if (error == 0)
    return -1;

  message = message_create_string(error);
  if (message == 0)
    return -1;

  message->type = MSG_TYPE_ERROR;

  if (message_send(fd, message) == -1) {
    message_destroy(message);
    return -1;
  }

  message_destroy(message);
  return 0;
}

int send_ping(int fd, int count) {
  message_t *message;

  message = message_create(MSG_TYPE_PING, sizeof(count));
  if (message == 0)
    return -1;
  memcpy(message->data, &count, sizeof(count));

  if (message_send(fd, message) == -1) {
    message_destroy(message);
    return -1;
  }

  message_destroy(message);
  return 0;
}

int send_quit(int fd) {
  message_t *message;

  message = message_create(MSG_TYPE_QUIT, 0);
  if (message == 0)
    return -1;

  if (message_send(fd, message) == -1) {
    message_destroy(message);
    return -1;
  }

  message_destroy(message);
  return 0;
}

int send_blob(int fd, const void *data, size_t size) {
  message_t *message;

  if (data == 0 || size == 0)
    return -1;

  message = message_create(MSG_TYPE_BLOB, size);
  if (message == 0)
    return -1;
  memcpy(message->data, data, size);

  if (message_send(fd, message) == -1) {
    message_destroy(message);
    return -1;
  }

  message_destroy(message);
  return 0;
}

int send_file_head(int fd, const file_t *file) {
  message_t *message;

  if (file == 0 || file->filename_size == 0 || file->filename == 0) {
    errno = EINVAL;
    return -1;
  }

  message = message_create(MSG_TYPE_FILE, sizeof(file->filename_size) + file->filename_size + sizeof(file->filesize));
  if (message == 0)
    return -1;

  memcpy(message->data, &file->filename_size, sizeof(file->filename_size));
  memcpy((char *)message->data + sizeof(file->filename_size), file->filename, file->filename_size);
  memcpy((char *)message->data + sizeof(file->filename_size) + file->filename_size, &file->filesize,
         sizeof(file->filesize));

  if (message_send(fd, message) == -1) {
    message_destroy(message);
    return -1;
  }
  message_destroy(message);
  return 0;
}

int send_collection(int fd) {
  message_t *message;

  message = message_create(MSG_TYPE_COLLECTION, 0);
  if (message == 0)
    return -1;

  if (message_send(fd, message) == -1) {
    message_destroy(message);
    return -1;
  }

  message_destroy(message);
  return 0;
}

int send_end_collection(int fd) {
  message_t *message;

  message = message_create(MSG_TYPE_END_COLLECTION, 0);
  if (message == 0)
    return -1;

  if (message_send(fd, message) == -1) {
    message_destroy(message);
    return -1;
  }

  message_destroy(message);
  return 0;
}