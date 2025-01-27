#include <stdio.h>
#include <unistd.h>

#include <string.h>
#include <employees.h>
#include <common.h>

int write_employees(struct employee_t **employeeout, char *name, char *address, int hours, unsigned int count)
{
  strcpy((*employeeout)[count - 1].name, name);
  strcpy((*employeeout)[count - 1].address, address);
  (*employeeout)[count - 1].hours = hours;

  return STATUS_SUCCESS;
}

int read_employees(struct employee_t **employeeout, int fd, unsigned int count)
{
  int i;
  for (i = 0; i < count; i++)
  {
    if (read(fd, &(*employeeout)[i], sizeof(struct employee_t)) == -1)
    {
      perror("Failed to read information about employee");
      return STATUS_FAILED;
    }
  }

  return STATUS_SUCCESS;
}

struct employee_t *query_employee(struct employee_t **employeeout, char *employeeName, unsigned int count)
{
  int i;
  for (i = 0; i < count; i++)
  {
    printf("%s %s\n", (*employeeout)[i].name, employeeName);
    if (strcmp((*employeeout)[i].name, employeeName) == 0)
    {
      printf("hi\n");
      return employeeout[i];
    }
  }

  return NULL;
}