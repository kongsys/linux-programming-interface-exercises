#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int dup(int oldfd) {
  if (fcntl(oldfd, F_GETFD) < 0) {
    errno = EBADF;
    return -1;
  }
  return fcntl(oldfd, F_DUPFD, 0);
}

int dup2(int oldfd, int newfd) {
  if (fcntl(oldfd, F_GETFD) < 0) {
    errno = EBADF;
    return -1;
  }
  if (oldfd == newfd) {
    return newfd;
  }
  int ret = fcntl(newfd, F_GETFD);

  if (ret < 0 && errno != EBADF) {
    return -1;
  } else if (ret >= 0) {
    close(newfd);
  }

  return fcntl(oldfd, F_DUPFD, newfd);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: %s <filename>", argv[0]);
    return 1;
  }
  char buf[30];

  memset(buf, '\0', sizeof(buf));

  int fd1 = open(argv[1], O_RDONLY);
  int fd2 = dup(fd1);

  read(fd1, &buf[0], 20);
  printf("%s\n", buf);
  read(fd2, &buf[0], 20);
  printf("%s\n", buf);
  close(fd1);
  close(fd2);
  return 0;
}
