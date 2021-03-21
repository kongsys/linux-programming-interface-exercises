#include <sys/uio.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>
#include <assert.h>

ssize_t readv(int fd, const struct iovec *iov, int iov_cnt) {
  if (iov_cnt < 0) {
    errno = EINVAL;
    return -1;
  }
  ssize_t total_bytes = 0;
  struct iovec *vec = NULL;

  unsigned long long cnt = 0;
  for (int i = 0; i < iov_cnt; i++) {
    vec = (struct iovec *)&iov[i];
    cnt += vec->iov_len;
    if (cnt > SSIZE_MAX) {
      errno = EINVAL;
      return -1;
    }
  }

  for (int i = 0; i < iov_cnt; i++) {
    vec = (struct iovec *)&iov[i];
    ssize_t n = read(fd, vec->iov_base, vec->iov_len);
    
    if (total_bytes < 0) {
      return n;
    } else {
      total_bytes += n;
      if (n > vec->iov_len) {
        break;
      }
    }
  }


  return total_bytes;
}

ssize_t writev(int fd, const struct iovec *iov, int iov_cnt) {
  if (iov_cnt < 0) {
    errno = EINVAL;
    return -1;
  }
  ssize_t total_bytes = 0;
  struct iovec *vec = NULL;

  unsigned long long cnt = 0;
  for (int i = 0; i < iov_cnt; i++) {
    vec = (struct iovec *)&iov[i];
    cnt += vec->iov_len;
    if (cnt > SSIZE_MAX) {
      errno = EINVAL;
      return -1;
    }
  }

  for (int i = 0; i < iov_cnt; i++) {
    vec = (struct iovec *)&iov[i];
    ssize_t n = write(fd, vec->iov_base, vec->iov_len);
    
    if (total_bytes < 0) {
      return n;
    } else {
      total_bytes += n;
      if (n > vec->iov_len) {
        break;
      }
    }
  }


  return total_bytes;
}

#define NUMBER_VECS 100
#define CHUNK_SIZE 16
static struct iovec read_iovecs[NUMBER_VECS];
static struct iovec write_iovecs[NUMBER_VECS];

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }
  for (int i = 0; i < NUMBER_VECS; i++) {
    void *mem;
    mem = malloc(CHUNK_SIZE);
    memset(mem, 0, CHUNK_SIZE);
    read_iovecs[i].iov_base = mem;
    read_iovecs[i].iov_len = CHUNK_SIZE;

    mem = malloc(CHUNK_SIZE);
    memset(mem, i, CHUNK_SIZE);
    write_iovecs[i].iov_base = mem;
    write_iovecs[i].iov_len = CHUNK_SIZE;
  }  

  int fd = open(argv[1], (O_WRONLY | O_CREAT), (S_IRUSR | S_IWUSR));

  ssize_t len_written = writev(fd, &write_iovecs[0], NUMBER_VECS);
  assert(len_written == (NUMBER_VECS * CHUNK_SIZE));
  close(fd);

  fd = open(argv[1], O_RDONLY);
  ssize_t len_read = readv(fd, &read_iovecs[0], NUMBER_VECS);
  assert (len_read == len_written);

  for (int i = 0; i < NUMBER_VECS; i++) {
    assert(memcmp((void*)write_iovecs[i].iov_base, (void*)read_iovecs[i].iov_base, CHUNK_SIZE) == 0);
  }

  printf("Done... no assertion errors\n");
  return 0;
}
