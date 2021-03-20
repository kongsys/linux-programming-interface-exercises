#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#ifndef BUF_SIZE
#define BUF_SIZE 4096
#endif

int main(int argc, char **argv) {
  int input_fd = -1;
  int output_fd = -1;
  const int open_flags = O_CREAT | O_WRONLY | O_TRUNC;
  const mode_t file_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IWOTH;
  char buf[BUF_SIZE];

  if (argc != 3 || strcmp(argv[1], "--help") == 0) {
    printf("Usage: ./%s <src> <dst>\n", argv[0]);
    return 1;
  }
  input_fd = open(argv[1], O_RDONLY);
  if (input_fd < 0) {
    fprintf(stderr, "Failed to open input file: %s\n", strerror(errno));
    return 1;
  }
  output_fd = open(argv[2], open_flags, file_perms);
  if (output_fd < 0) {
    fprintf(stderr, "Failed to open output file: %s\n", strerror(errno));
    return 1;
  }

  int i = 0;
  unsigned long long hole_size = 0;
  ssize_t reads = 0;
  while ((reads = read(input_fd, buf, BUF_SIZE)) > 0) {
    for ( i = 0; i < reads; i++) {
      if (buf[i] == '\0') {
        hole_size++;
      } else if (hole_size > 0) {
        lseek(output_fd, hole_size, SEEK_CUR);
        write(output_fd, &buf[i], 1);
        hole_size = 0;
      } else {
        write(output_fd, &buf[i], 1);
      }
    }
  }

  close(input_fd);
  close(output_fd);

  return 0;
}
