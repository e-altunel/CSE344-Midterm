#include <errno.h>
#include <fcntl.h>
#include <file_helper.h>
#include <globals.h>
#include <message.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdio.h>

static int lock_file(int fd, int type) {
  struct flock lock;

  lock.l_type   = type;
  lock.l_whence = SEEK_SET;
  lock.l_start  = 0;
  lock.l_len    = 0;
  lock.l_pid    = getpid();

  return fcntl(fd, F_SETLKW, &lock);
}

int file_download(int recv) {
  file_t     file;
  message_t *message;
  int        ret = 0;
  int        fd;
  message_t *head = message_recv(recv);

  if (head == 0)
    return -1;

  if (head->type != MSG_TYPE_FILE) {
    message_destroy(head);
    return -1;
  }

  file.filename_size = *(size_t *)head->data;
  file.filename      = malloc(file.filename_size + 1);
  if (file.filename == 0) {
    message_destroy(head);
    return -1;
  }
  memcpy(file.filename, (char *)head->data + sizeof(size_t), file.filename_size);
  file.filename[file.filename_size] = 0;

  file.filesize = *(size_t *)((char *)head->data + sizeof(size_t) + file.filename_size);

  printf("Downloading %s (%lu bytes)\n", file.filename, file.filesize);

  fd = open(file.filename, O_WRONLY | O_CREAT, 0644);
  if (fd == -1)
    return -1;

  if (lock_file(fd, F_WRLCK) == -1) {
    close(fd);
    return -1;
  }

  while (ret == 0) {
    ssize_t bytes_written;
    message = message_recv(recv);
    if (message == 0) {
      message_destroy(message);
      ret = -1;
      break;
    }
    if (message->size == 0) {
      message_destroy(message);
      break;
    }
    if (message->type != MSG_TYPE_BLOB) {
      message_destroy(message);
      ret = -1;
      break;
    }
    bytes_written = write(fd, message->data, message->size);
    if (bytes_written == -1) {
      message_destroy(message);
      ret = -1;
      break;
    }
    message_destroy(message);
  }

  lock_file(fd, F_UNLCK);
  close(fd);
  return ret;
}

int file_upload(int send, const char *filename) {
  int        fd;
  file_t     file;
  message_t *message;
  int        ret = 0;

  file.filesize      = 0;
  file.filename_size = strlen(filename);
  file.filename      = malloc(file.filename_size + 1);
  if (file.filename == 0)
    return -1;

  memcpy(file.filename, filename, file.filename_size);

  fd = open(filename, O_RDONLY);
  if (fd == -1)
    return -1;

  if (lock_file(fd, F_RDLCK) == -1) {
    close(fd);
    return -1;
  }

  file.filesize = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  if (send_file_head(send, &file) == -1) {
    close(fd);
    ret = -1;
  }

  printf("Uploading %s (%lu bytes | %lu chunks)\n", file.filename, file.filesize, file.filesize / BUFFER_SIZE);

  message = message_create(MSG_TYPE_BLOB, BUFFER_SIZE);
  if (message == 0) {
    close(fd);
    ret = -1;
  }

  while (ret == 0) {
    ssize_t bytes_read = read(fd, message->data, BUFFER_SIZE);
    if (bytes_read == -1) {
      ret = -1;
      break;
    }
    if (bytes_read == 0)
      break;
    message->size = bytes_read;
    if (message_send(send, message) == -1) {
      ret = -1;
      break;
    }

    if (bytes_read < BUFFER_SIZE)
      break;
  }
  message->size = 0;
  message_send(send, message);

  lock_file(fd, F_UNLCK);

  message_destroy(message);
  close(fd);
  return ret;
}
