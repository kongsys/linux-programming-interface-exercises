#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static int append_mode = 0;

int parse_args(int argc, char **argv) {
  char c;
  while ((c = getopt(argc, argv, "a")) != -1) {
    switch (c) {
      case 'a': append_mode = 1; break;
      case '?': {
                if (isprint(optopt)) {
                  fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                  fprintf(stderr, "Unknow option character `\\x%x'.\n", optopt);
                }
                return 1;
              }
      default:
        abort();
        break;
    }
  }
  return 0;
}

void close_files(int nums, FILE *files[]) {
  for (int i = 0; i < nums; i++) {
    fclose(files[i]);
  }
}

int main(int argc, char **argv) {
  char buf[100];
  size_t len = 0;
  char *file_mode = NULL;
  int i = 0;
  FILE *files[20];

  if (files == NULL) {
    fprintf(stderr, "Unable to allocate file buffer space\n");
    return 1;
  }

  if (parse_args(argc, argv)) {
    return 1;
  }

  file_mode = append_mode ? "a" : "w";

  const int num_files = argc - optind;

  for (i = optind; i < argc; i++) {
    FILE *pFile = fopen(argv[i], file_mode);
    if (pFile == NULL) {
      fprintf(stderr, "Unable to open file %s for mode %s", argv[i], file_mode);
      close_files(i + 1, files);
      return 0;
    }
    files[i-optind] = pFile;
  }

  FILE *not_stdin = fopen("tee.c", "r");

  while ((len = fread(&buf[0], 1, sizeof(buf), not_stdin)) > 0) {
    fwrite(&buf[0], 1, len, stdout);
    for (i = 0; i < num_files; i++) {
      fwrite(&buf[0], 1, len, files[i]);
    }
  }

  close_files(num_files, files);
  return 0;
}
