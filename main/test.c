#include <fcntl.h>
#include <file_helper.h>
#include <message.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int fd = open("test.txt", O_CREAT | O_WRONLY, 0666);

  command_t command = {0};

  command.type = CMD_TYPE_HELP;
  command.data = 0;
  command.size = 0;
  send_command(fd, &command);

  perror("Failed to send command");

  close(fd);

  return 0;
}