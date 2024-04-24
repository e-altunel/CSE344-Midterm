#ifndef INC_DEFINES
#define INC_DEFINES

#define SERVER_FIFO_PATH "/tmp/cse344_midterm_server_fifo"
#define CLIENT_FIFO_PATH "/tmp/cse344_midterm_client_fifo"
#define MAX_CLIENTS 100

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 256
#elif
#if BUFFER_SIZE < 50
#error "Buffer size must be at least 50"
#endif
#endif

#endif /* INC_DEFINES */
