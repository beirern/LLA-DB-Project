#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include <common.h>

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
