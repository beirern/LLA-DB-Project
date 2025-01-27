#ifndef HEADERS_H
#define HEADERS_H

#define CURRENT_VERSION 1
#define MAGIC 0x21d16ba9 // random

struct dbheader_t
{
  unsigned short count;
  unsigned short version;
  unsigned int filesize;
  unsigned int magic;
};

int read_default_headers(struct dbheader_t **headerout);
int read_headers_from_file(int fd, struct dbheader_t **headerout);
int get_size_of_file(int fd);
void print_header(struct dbheader_t *header);

#endif