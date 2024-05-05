#ifndef INC_COMMAND
#define INC_COMMAND

#include <stddef.h>

typedef enum {
  CMD_TYPE_HELP,
  CMD_TYPE_LIST,
  CMD_TYPE_READF,
  CMD_TYPE_WRITET,
  CMD_TYPE_UPLOAD,
  CMD_TYPE_DOWNLOAD,
  CMD_TYPE_ARCHIVE,
  CMD_TYPE_KILL,
  CMD_TYPE_QUIT
} command_type_t;

typedef struct {
  command_type_t type;
  size_t         size;
  void          *data;
} command_t;

void send_help(int send, void *data);
void send_list(int send, const char *dirname);
void send_readf(int send, void *data);
void send_writet(int send, void *data);
void send_upload(int send, void *data);
void send_download(int send, void *data);
void send_archive(int send, void *data);
void send_kill(int send);

#endif /* INC_COMMAND */
