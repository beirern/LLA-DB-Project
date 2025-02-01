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
#include <string.h>
#include <errno.h>

#include <file.h>
#include <headers.h>
#include <common.h>
#include <employees.h>

int main(int argc, char *argv[])
{
  // track if we're creating a new file and what the name
  // should be
  int newfile = 0;
  char *fileName = NULL;
  int getOptCode;
  char *employeeName = NULL;
  char *employeeInfo = NULL;
  int newEmployee = 0;
  char *name = NULL;
  char *address = NULL;
  int hours = 0;
  int listEmployees = 0;
  unsigned int newHours = 0;
  int remove = 0;

  // Double pointer so that we can modify and pass reference to
  // headers
  struct dbheader_t **headerout = malloc(sizeof(struct dbheader_t *));
  struct employee_t **employeeout = malloc(sizeof(struct employee_t *));

  while ((getOptCode = getopt(argc, argv, "nf:q:e:lu:r")) != -1)
  {
    switch (getOptCode)
    {
    case 'n':
      newfile = 1;
      break;
    case 'f':
      fileName = optarg;
      break;
    case 'q':
      employeeName = optarg;
      break;
    case 'e':
      employeeInfo = optarg;
      newEmployee = 1;
      char *delim = ",";
      name = strtok(employeeInfo, delim);
      if (name == NULL)
      {
        fprintf(stderr, "Failed to parse out the name of the employee!\n");
        fprintf(stderr, "Employee attributes should be comma separated\n");
        abort();
      }
      address = strtok(NULL, delim);
      if (address == NULL)
      {
        fprintf(stderr, "Failed to parse out the address of the employee!\n");
        fprintf(stderr, "Employee attributes should be comma separated\n");
        abort();
      }
      char *hoursstr = strtok(NULL, delim);
      if (hoursstr == NULL)
      {
        fprintf(stderr, "Failed to parse out the hours of the employee!\n");
        fprintf(stderr, "Employee attributes should be comma separated\n");
        abort();
      }
      hours = atoi(hoursstr);
      if (hours == 0)
      {
        perror("Failed to convert the hours of employee to a string!");
        fprintf(stderr, "Employee attributes should be comma separated\n");
        abort();
      }
      break;
    case 'l':
      listEmployees = 1;
      break;
    case 'u':
      char *resultPointer = NULL;
      newHours = strtoul(optarg, &resultPointer, 10);
      errno = 0; // strtoul sets errno to non 0 on failure
      if (resultPointer == optarg || *resultPointer != '\0' || errno != 0)
      {
        perror("Failed to convert hours to update to an unsigned int");
        abort();
      }
      break;
    case 'r':
      remove = 1;
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
    if (read_default_headers(headerout) == STATUS_FAILED)
    {
      close(fd);
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

    if (read_headers_from_file(fd, headerout) == STATUS_FAILED)
    {
      close(fd);
      abort();
    }
  }

  printf("Initial DB Information\n");
  print_header(*headerout);

  *employeeout = calloc((*headerout)->count, sizeof(struct employee_t));
  if (*employeeout == NULL)
  {
    perror("Failed to allocate memory for employees");
    close(fd);
    abort();
  }
  if (read_employees(employeeout, fd, (*headerout)->count) == STATUS_FAILED)
  {
    close(fd);
    abort();
  }

  if (newEmployee)
  {
    (*headerout)->count += 1;
    *employeeout = reallocarray(*employeeout, (*headerout)->count, sizeof(struct employee_t));
    write_employees(employeeout, name, address, hours, (*headerout)->count);
    (*headerout)->filesize += sizeof(struct employee_t);
  }

  if (employeeName)
  {
    struct employee_t *e = query_employee(employeeout, employeeName, (*headerout)->count);

    if (e == NULL)
    {
      printf("Failed to find employee\n");
    }
    else
    {
      printf("Found employee with hours: %d\n", e->hours);
      if (newHours)
      {
        e->hours = newHours;
        printf("Updated %s's hours to %d\n", e->name, e->hours);
      }

      if (remove)
      {
        remove_employee(employeeout, employeeName, (*headerout)->count);
        (*headerout)->count -= 1;
        *employeeout = reallocarray(*employeeout, (*headerout)->count, sizeof(struct employee_t));
      }
    }
  }

  if (listEmployees)
  {
    int i;
    for (i = 0; i < (*headerout)->count; i++)
    {
      printf("Employee %d\n", i);
      printf("\tName %s\n", (*employeeout)[i].name);
      printf("\tAddress %s\n", (*employeeout)[i].address);
      printf("\tHours Worked %d\n", (*employeeout)[i].hours);
    }
  }

  printf("Writing the following file\n");
  print_header(*headerout);

  if (write_to_file(fd, headerout, employeeout) == STATUS_FAILED)
  {
    close(fd);
    abort();
  }

  free(*headerout);
  free(*employeeout);
  free(headerout);
  free(employeeout);
  close(fd);

  return 0;
}