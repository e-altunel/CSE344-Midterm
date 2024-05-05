#ifndef INC_SERVER_HELPER
#define INC_SERVER_HELPER

#include <message.h>
#include <pid_array.h>

int  accept_client(pid_t client_pid, pid_array_t *pid_array, int *isParent, const char *dirname);
int  child_job(pid_t client_pid, int recv, int send, const char *dirname);
void handle_message_server(const message_t *message, const char *dirname, int send);
void handle_command_server(void *command, const char *dirname, int send);

#endif /* INC_SERVER_HELPER */
