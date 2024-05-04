#ifndef INC_SERVER_HELPER
#define INC_SERVER_HELPER

#include <pid_array.h>

int accept_client(pid_t client_pid, pid_array_t *pid_array, int *isParent);
int child_job(pid_t client_pid);

#endif /* INC_SERVER_HELPER */
