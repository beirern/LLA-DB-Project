#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <common.h>
#include <headers.h>
#include <file.h>

// This file handles makihg and parsing our file headers

// Helper method to pring what a header structure holds
void print_header(struct dbheader_t *header)
{
  printf("\tcount: %d\n", header->count);
  printf("\tversion: %d\n", header->version);
  printf("\tfilesize: %d\n", header->filesize);
  printf("\tmagic: %d\n", header->magic);
}

// Sets *headerout to header struct with default headers
int intialize_default_headers(struct dbheader_t **headerout)
{
  *headerout = malloc(sizeof(struct dbheader_t));
  if (*headerout == NULL)
  {
    perror("Failed to allocate memory for default headers");
    return STATUS_FAILED;
  }
  (*headerout)->count = 0;
  (*headerout)->version = CURRENT_VERSION;
  (*headerout)->filesize = sizeof(struct dbheader_t);
  (*headerout)->magic = MAGIC;

  return STATUS_SUCCESS;
}

// Reads in headers in a file
// Fails if
// The size of the file via stat is not the same as the filesize in the header
// The version in the header file is not the same as the version the program is running on
// The file magic value is not the same as the one in code
int read_headers_from_file(int fd, struct dbheader_t **headerout)
{
  struct dbheader_t *p_header = malloc(sizeof(struct dbheader_t));

  if (p_header == NULL)
  {
    perror("Malloc failed");
    return STATUS_FAILED;
  }
  if (read(fd, p_header, sizeof(struct dbheader_t)) == -1)
  {
    perror("Reading the headers failed");
    return STATUS_FAILED;
  }

  p_header->count = ntohs(p_header->count);
  p_header->version = ntohs(p_header->version);
  p_header->filesize = ntohl(p_header->filesize);
  p_header->magic = ntohl(p_header->magic);

  if (p_header->version != CURRENT_VERSION)
  {
    fprintf(stderr, "Version of file (%d) is not the same as program's (%d)\n", p_header->version, CURRENT_VERSION);
    return STATUS_FAILED;
  }

  if (p_header->magic != MAGIC)
  {
    fprintf(stderr, "Magic of file (%d) is not the same as program's (%d)\n", p_header->magic, MAGIC);
    return STATUS_FAILED;
  }

  off_t file_size = get_size_of_file(fd);
  if (file_size != p_header->filesize)
  {
    fprintf(stderr, "Size of file according to fstat (%ld) does not match the header size (%d)\n", file_size, p_header->filesize);
    return STATUS_FAILED;
  }

  // Set header pointer to the header with values set
  *headerout = p_header;

  return STATUS_SUCCESS;
}