#ifndef INC_FIFO_HELPER
#define INC_FIFO_HELPER

#define GLOBAL_SERVER_QUEUE_FORMAT "/tmp/cse344_server_queue_%d"
#define PROCESS_COMM_FORMAT "/tmp/cse344_client_server_channel_%d_%d"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

#include <fcntl.h>

int   create_server_queue();
int   destroy_server_queue();
pid_t connect_server(pid_t server_pid, int flags, int *recv, int *send);
int   connect_server_queue(pid_t server_pid, int flags);
int   connect_process_comm(pid_t from_pid, pid_t to_pid, int flags);
int   create_process_comm(pid_t from_pid, pid_t to_pid, int flags);
int   destroy_process_comm(pid_t from_pid, pid_t to_pid);

#endif /* INC_FIFO_HELPER */
