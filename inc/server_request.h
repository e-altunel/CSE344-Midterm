#ifndef INC_SERVER_REQUEST
#define INC_SERVER_REQUEST

#include <sys/types.h>

typedef struct server_connect_request_s {
  pid_t client_pid;
} server_connect_request_t;

typedef struct server_request_s {
  pid_t client_pid;
  char  message[256];
} server_request_t;

#endif /* INC_SERVER_REQUEST */
