#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <common.h>
#include <headers.h>

// This file handles makihg and parsing our file headers

// Helper method to pring what a header structure holds
void print_header(struct dbheader_t *header)
{
  printf("count: %d\n", ntohs(header->count));
  printf("version: %d\n", ntohs(header->version));
  printf("filesize: %d\n", ntohl(header->filesize));
  printf("magic: %d\n", ntohl(header->magic));
}

// Writes the headers to a file. If this is a new file and there
// are no headers that were read in then passes the default values
// defined in headers.h
int write_headers(int fd, struct dbheader_t **headerout)
{
  // If this is null then we did not read in any headers, i.e. this is a new file
  if (*headerout == NULL)
  {
    struct dbheader_t header =
        {
            htons(0),
            htons(CURRENT_VERSION),
            htonl(sizeof(struct dbheader_t)),
            htonl(MAGIC)};

    // Since this is a new file we do not need to lseek the beginning of it
    write(fd, &header, sizeof(struct dbheader_t));
    *headerout = &header;
  }
  else
  {
    (*headerout)->count = htons((*headerout)->count);
    (*headerout)->version = htons((*headerout)->version);
    (*headerout)->filesize = htonl((*headerout)->filesize);
    (*headerout)->magic = htonl((*headerout)->magic);

    if (lseek(fd, 0, SEEK_SET) == -1)
    {
      perror("Failed to reset position in file for writing");
      return STATUS_FAILED;
    }
    write(fd, *headerout, sizeof(struct dbheader_t));
  }

  return STATUS_SUCCESS;
}

// Gets the size of a file determined by stat
int get_size_of_file(int fd)
{
  struct stat *p_stat_info = malloc(sizeof(struct stat));
  if (p_stat_info == NULL)
  {
    perror("Malloc failed");
    return STATUS_FAILED;
  }

  int stat_status = fstat(fd, p_stat_info);
  if (stat_status == -1)
  {
    perror("Failed to stat file");
    return STATUS_FAILED;
  }

  return p_stat_info->st_size;
}

// Reads in headers in a file
// Fails if
// The size of the file via stat is not the same as the filesize in the header
// The version in the header file is not the same as the version the program is running on
int read_headers(int fd, struct dbheader_t **headerout)
{
  struct dbheader_t *p_header = malloc(sizeof(struct dbheader_t));

  if (p_header == NULL)
  {
    perror("Malloc failed");
    return STATUS_FAILED;
  }
  read(fd, p_header, sizeof(struct dbheader_t));

  p_header->count = ntohs(p_header->count);
  p_header->version = ntohs(p_header->version);
  p_header->filesize = ntohl(p_header->filesize);
  p_header->magic = ntohl(p_header->magic);

  if (p_header->version != CURRENT_VERSION)
  {
    fprintf(stderr, "Version of file (%d) is not the same as program's (%d)", p_header->version, CURRENT_VERSION);
    return STATUS_FAILED;
  }

  if (get_size_of_file(fd) != p_header->filesize)
  {
    fprintf(stderr, "Size of file according to fstat (%d) does not match the header size (%d)\n", get_size_of_file(fd), p_header->filesize);
    return STATUS_FAILED;
  }

  *headerout = p_header;

  return STATUS_SUCCESS;
}