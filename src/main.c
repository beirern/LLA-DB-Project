#include <stdio.h>

// abort()
#include <stdlib.h>

// Getopt
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>

// Open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <file.h>
#include <headers.h>
#include <common.h>

int main(int argc, char *argv[])
{

  // track if we're creating a new file and what the name
  // should be
  int newfile = 0;
  char *fileName = NULL;
  int getOptCode;

  while ((getOptCode = getopt(argc, argv, "nf:")) != -1)
  {
    switch (getOptCode)
    {
    case 'n':
      newfile = 1;
      break;
    case 'f':
      fileName = optarg;
      break;
    case '?':
      if (optopt == 'c')
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt))
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf(stderr,
                "Unknown option character `\\x%x'.\n",
                optopt);
      return 1;
    default:
      abort();
    }
  }

  if (fileName == NULL)
  {
    fprintf(stderr, "File required! Usage: %s -f <file-name>\n", argv[0]);
    return -1;
  }

  int fd;

  if (newfile)
  {
    fd = create_db_file(fileName);
    if (fd == -1)
    {
      abort();
    }
  }
  else
  {
    fd = open_db_file(fileName);
    if (fd == -1)
    {
      abort();
    }
  }

  // Double pointer so that we can modify and pass reference to
  // headers
  struct dbheader_t **headerout = malloc(sizeof(struct dbheader_t *));

  // Dont read headers if this is a new file
  if (!newfile)
  {
    int read_status = read_headers(fd, headerout);
    if (read_status == STATUS_FAILED)
    {
      abort();
    }
  }

  write_headers(fd, headerout);

  print_header(*headerout);

  return 0;
}