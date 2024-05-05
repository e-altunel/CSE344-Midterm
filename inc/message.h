#ifndef INC_MESSAGE
#define INC_MESSAGE

#include <command.h>
#include <file_helper.h>
#include <stddef.h>
#include <sys/param.h>
#include <time.h>

typedef enum {
  MSG_TYPE_COMMAND,
  MSG_TYPE_QUIT,
  MSG_TYPE_PING,
  MSG_TYPE_ERROR,
  MSG_TYPE_STRING,
  MSG_TYPE_BLOB,
  MSG_TYPE_FILE,
  MSG_TYPE_COLLECTION,
  MSG_TYPE_END_COLLECTION
} message_type_t;

typedef struct {
  message_type_t type;
  time_t         timestamp;
  size_t         size;
  void          *data;
} message_t;

message_t *message_create(message_type_t type, size_t size);
message_t *message_create_string(const char *string);

void       message_destroy(message_t *message);
int        message_send(int fd, const message_t *message);
message_t *message_recv(int fd);
void       message_print(const message_t *message);
void       message_print_data(const message_t *message);
void       time_print(const time_t time);

int send_command(int fd, const command_t *command);
int send_quit(int fd);
int send_ping(int fd, int count);
int send_string(int fd, const char *string);
int send_error(int fd, const char *error);
int send_blob(int fd, const void *data, size_t size);
int send_file_head(int fd, const file_t *file);
int send_collection(int fd);
int send_end_collection(int fd);

#endif /* INC_MESSAGE */
