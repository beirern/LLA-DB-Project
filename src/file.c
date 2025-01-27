#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <common.h>
#include <headers.h>
#include <employees.h>

// These functions are to handle opening and creating a db file

// Opens a file with the given filename
// return int fd File descriptor or STATUS_FAILED indicating an error
int open_db_file(char *fileName)
{
  int fd = open(fileName, O_RDWR);
  if (fd == -1)
  {
    perror("Issue opening file");
    return STATUS_FAILED;
  }

  return fd;
}

// Creates a file with the given filename given the file does not already exist
// return int fd File descriptor or STATUS_FAILED indicating an error
int create_db_file(char *fileName)
{
  int fd = open(fileName, O_RDONLY);

  if (fd != -1)
  {
    printf("File already exists %s\n", fileName);
    return STATUS_FAILED;
  }

  fd = open(fileName, O_CREAT | O_RDWR, 0644);
  if (fd == -1)
  {
    perror("Issue creating file");
    return STATUS_FAILED;
  }

  return fd;
}

// int write_to_file(int fd, struct dbheader_t **headerout)
int write_to_file(int fd, struct dbheader_t **headerout, struct employee_t **employeeout)
{
  if (lseek(fd, 0, SEEK_SET) == -1)
  {
    perror("Failed to reset position in file for writing");
    return STATUS_FAILED;
  }

  unsigned int nonEndianCount = (*headerout)->count;

  (*headerout)->count = htons((*headerout)->count);
  (*headerout)->version = htons((*headerout)->version);
  (*headerout)->filesize = htonl((*headerout)->filesize);
  (*headerout)->magic = htonl((*headerout)->magic);

  if (write(fd, *headerout, sizeof(struct dbheader_t)) == -1)
  {
    perror("Failed to write headers to file");
    return STATUS_FAILED;
  }

  if (write(fd, *employeeout, sizeof(struct employee_t) * nonEndianCount) == -1)
  {
    perror("Failed to write employees to file");
    return STATUS_FAILED;
  }

  return STATUS_SUCCESS;
}
