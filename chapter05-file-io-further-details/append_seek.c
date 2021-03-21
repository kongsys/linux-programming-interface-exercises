#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CONTENT "Hello, World!"


int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Must specify a file (argument 1)\n");
    return 1;
  }
  int fd = open(argv[1], O_RDWR | O_APPEND);

  lseek(fd, 0, SEEK_SET);

  write(fd, CONTENT, sizeof(CONTENT));
  close(fd);
  return 0;
}
