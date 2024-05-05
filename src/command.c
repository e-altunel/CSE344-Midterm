#include <command.h>
#include <command_helper.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <globals.h>
#include <message.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int lock_file(int fd, int type) {
  struct flock lock;

  lock.l_type   = type;
  lock.l_whence = SEEK_SET;
  lock.l_start  = 0;
  lock.l_len    = 0;
  lock.l_pid    = getpid();

  return fcntl(fd, F_SETLKW, &lock);
}

void send_help(int send, void *data) {
  size_t str_size = 0;
  char  *str      = 0;

  if (data == 0) {
    send_error(send, "Invalid command");
    errno = EINVAL;
    return;
  }

  str_size = get_size_t(&data);
  if (str_size != 0) {
    str = malloc(str_size + 1);
    if (str == 0) {
      send_error(send, "Memory allocation failed");
      errno = ENOMEM;
      return;
    }
    get_data(&data, str_size, str);
    str[str_size] = 0;
  } else {
    str = 0;
  }

  send_collection(send);
  if (str == 0 || !strcmp(str, "help"))
    send_string(send, "- help display the list of possible client requests\n");
  if (str == 0 || !strcmp(str, "list"))
    send_string(send, "- list sends a request to display the list of files in Servers directory(also displays the "
                      "list received from the Server)\n");
  if (str == 0 || !strcmp(str, "readF"))
    send_string(send, "readF <file> <line #> requests to display the # line of the <file>, if no line number is given "
                      "the whole contents of the file is requested (and displayed on the client side)\n");
  if (str == 0 || !strcmp(str, "writeT"))
    send_string(send, "writeT <file> <line #> <string>:request to write the  content of \"string\" to the  #th  line "
                      "the <file>, if the line # is not given writes the end of file. If the file does not exists in "
                      "Servers directory creates and edits thefile at the same time\n");
  if (str == 0 || !strcmp(str, "upload"))
    send_string(send, "upload <file>uploads the file from the current working directory of client to the Servers"
                      " directory (beware of the cases no file in clients current working directory and file with"
                      " the same name on Servers side)\n");
  if (str == 0 || !strcmp(str, "download"))
    send_string(send, "download <file>request to receive <file> from Servers directory to client side\n");
  if (str == 0 || !strcmp(str, "archServer"))
    send_string(send, "archServer <fileName>.tarUsing fork, exec and tar utilities create a child process that will"
                      " collect all the files currently available on the the  Server side and store them in the "
                      "<filename>.tar archive\n");
  if (str == 0 || !strcmp(str, "killServer"))
    send_string(send, "killServer "
                      "Sends a kill request to the Server\n");
  if (str == 0 || !strcmp(str, "quitServer"))
    send_string(send, "quitServer write request to Server side log file and quits\n");

  if (str != 0)
    free(str);

  send_end_collection(send);
}

void send_list(int send, const char *dirname) {
  DIR           *dir   = 0;
  struct dirent *entry = 0;

  if (dirname == 0) {
    send_error(send, "Invalid directory name");
    errno = EINVAL;
    return;
  }

  dir = opendir(dirname);
  if (dir == 0) {
    send_error(send, "Failed to open directory");
    return;
  }

  send_collection(send);
  while ((entry = readdir(dir)) != 0) {
    if (entry->d_name[0] == '.')
      continue;
    send_string(send, entry->d_name);
  }
  send_end_collection(send);

  closedir(dir);
}

void send_readf(int send, void *data) {
  char    buffer[BUFFER_SIZE + 1] = {0};
  size_t  str_size                = 0;
  char   *str                     = 0;
  size_t  line                    = 0;
  size_t  line_index              = 0;
  size_t  index                   = 0;
  int     new_line_found          = 0;
  ssize_t bytes_read              = 0;
  size_t  arg_count;
  int     fd;

  if (data == 0) {
    send_error(send, "Invalid command");
    errno = EINVAL;
    return;
  }

  arg_count = get_size_t(&data);
  if (arg_count != 2) {
    send_error(send, "Invalid number of arguments");
    errno = EINVAL;
    return;
  }

  str_size = get_size_t(&data);
  if (str_size != 0) {
    str = malloc(str_size + 1);
    if (str == 0) {
      send_error(send, "Memory allocation failed");
      errno = ENOMEM;
      return;
    }
    get_data(&data, str_size, str);
    str[str_size] = 0;
  } else {
    str = 0;
  }
  line = get_size_t(&data);

  fd = open(str, O_RDONLY);
  if (fd == -1) {
    send_error(send, "Failed to open file");
    free(str);
    return;
  }
  free(str);

  if (lock_file(fd, F_RDLCK) == -1) {
    send_error(send, "Failed to lock file");
    close(fd);
    return;
  }

  while (line_index < line) {
    ssize_t i  = 0;
    bytes_read = read(fd, buffer, BUFFER_SIZE);
    if (bytes_read == -1) {
      send_error(send, "Failed to read file");
      break;
    }
    if (bytes_read == 0)
      break;
    while (i < bytes_read) {
      if (buffer[i] == '\n') {
        line_index++;
        if (line_index == line) {
          index = i;
        }
      }
      i++;
    }
  }
  send_collection(send);
  while (!new_line_found) {
    ssize_t i = index;
    while (i < bytes_read) {
      if (buffer[i] == '\n') {
        new_line_found = 1;
        break;
      }
      i++;
    }
    if (new_line_found) {
      buffer[i] = 0;
      send_string(send, buffer + index);
      break;
    }
    send_string(send, buffer + index);
  }
  send_end_collection(send);

  if (lock_file(fd, F_UNLCK) == -1) {
    send_error(send, "Failed to lock file");
    close(fd);
    return;
  }
  close(fd);
}

void send_writet(int send, void *data) {
  (void)send;
  (void)data;
}

void send_upload(int send, void *data) {
  (void)send;
  (void)data;
}

void send_download(int send, void *data) {
  (void)send;
  (void)data;
}

void send_archive(int send, void *data) {
  (void)send;
  (void)data;
}

void send_kill(int send) {
  (void)send;
}