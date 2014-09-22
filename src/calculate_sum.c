#include <stdio.h>

unsigned calculate_sum(const char * filepath)
{
  unsigned sum = 0;

  FILE *fp = fopen(filepath, "rb");
  if (fp == NULL) {
    fprintf(stderr, "failed to open %s\n", filepath);
    return -1;
  }

  const int assumed_page_size = 4096;
  const int align = 2;
  while (!feof(fp)) {
    unsigned char buffer[assumed_page_size];
    int read_count = fread(buffer, 1, assumed_page_size, fp);
    int real_count = read_count;
    if (read_count < assumed_page_size) {
      int extra = (align - (read_count % align)) % align;
      real_count += extra;

      // make the content two-byte align, fill 0xff to the extra bytes
      while (extra > 0) {
        buffer[read_count + extra - 1] = 0xff;
        extra--;
      }
    }

    for (int i = 0; i < real_count; i++) {
      sum += buffer[i];
    }
  }

  fclose(fp);

  return sum;
}

unsigned calculate_sum_slow(const char * filepath)
{
  unsigned sum = 0;

  FILE *fp = fopen(filepath, "rb");
  if (fp == NULL) {
    fprintf(stderr, "failed to open %s\n", filepath);
    return -1;
  }

  // seek to the end of the file, in order to get the file size
  int ret = fseek(fp, 0L, SEEK_END);
  if (ret == 0) {
    // get the file size
    long size = ftell(fp);
    // 2 bytes align
    int extra_bytes = size % 2;

    // reset to the beginning of the file
    rewind(fp);
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
      sum += ch;
    }

    while (extra_bytes > 0) {
      sum += 0xff;
      extra_bytes--;
    }
  }


  fclose(fp);

  return sum;
}
