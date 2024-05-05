#ifndef INC_FILE_HELPER
#define INC_FILE_HELPER

#include <stddef.h>
#include <sys/param.h>

typedef struct {
  char  *filename;
  size_t filename_size;
  size_t filesize;
} file_t;

int file_download(int recv);
int file_upload(int send, const char *filename);

#endif /* INC_FILE_HELPER */
