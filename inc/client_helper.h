#ifndef INC_CLIENT_HELPER
#define INC_CLIENT_HELPER

#include <message.h>
#include <sys/types.h>

void handle_message_client(const message_t *message, int send, int recv);
void send_command_with_data(int argc, char **argv, int send);

#endif /* INC_CLIENT_HELPER */
