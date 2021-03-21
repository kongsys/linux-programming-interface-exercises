/*
 * This exercise is designed to demonstrate why the atomicity guaranteed
 * by opening a file with the O_APPEND flag is necessary.  Write a
 * program that takes up to three command-line arguments:
 *
 *     $ atomic_append <filename> <num-bytes> [x]
 *
 * This program should open the specified filename (creating it if
 * necessary) and append num-bytes bytes to the file by using write() to
 * write a byte at a time.  By default, the program should open the file
 * with the O_APPEND flag, but if a third command-line argument (x) is
 * supplied, th ethe O_APPEND flag should be omitted, and instead, the
 * program should perform and lseek(fd, 0, SEEK_END) call before each
 * write().  Run two instances of this program at the same time without
 * the x argument to write 1 million bytes to the same file:
 *
 *     $ atomic_append f1 1000000 & atomic_append f1 1000000
 *
 * Repeat the same steps, writing to a different file, but this time
 * specifying the x argument:
 *
 *     $ atomic_append f2 1000000 x & atomic_append f2 1000000 x
 *
 * List the sizes of the files f1 and f2 using `ls -l` and explain the
 * difference.
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

void usage(char **argv) {
  fprintf(stderr, "%s <filename> <num-bytes> [x]\n", argv[0]);
  fprintf(stderr, "   where x is only checked for existence\n");
}

int main(int argc, char **argv) {
  if (argc < 3 || argc > 4) {
    usage(argv);
    return 1;
  }

  int flags = O_RDWR | O_CREAT;

  if (argc == 3) {
    flags |= O_APPEND;
  }

  long len = atol(argv[2]);

  int fd = open(argv[1], flags, S_IRUSR | S_IWUSR);

  if (fd == -1) {
    fprintf(stderr, "Could not open specified filename (%s)\n", argv[1]);
    usage(argv);
    return 1;
  }

  for (long int i = 0; i < len; i++) {
    if (argc != 3) {
      lseek(fd, 0, SEEK_END);
    }
    write(fd, "i", 1);
  }
  close(fd);
  return 0;
}
