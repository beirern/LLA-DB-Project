#include <stdio.h>
#include <unistd.h>

#include <string.h>
#include <employees.h>
#include <common.h>

// Adds an employee to the array
int add_employee(struct employee_t **employeeout, char *name, char *address, int hours, unsigned int count)
{
  strcpy((*employeeout)[count - 1].name, name);
  strcpy((*employeeout)[count - 1].address, address);
  (*employeeout)[count - 1].hours = hours;

  return STATUS_SUCCESS;
}

// Reads the employees into memory
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

// Finds an employee
// Returns a pointer to the employee or NULL if not found
struct employee_t *query_employee(struct employee_t **employeeout, char *employeeName, unsigned int count)
{
  int i;
  for (i = 0; i < count; i++)
  {
    if (strcmp((*employeeout)[i].name, employeeName) == 0)
    {
      return &(*employeeout)[i];
    }
  }

  return NULL;
}

// Removes an employee from the employees array
int remove_employee(struct employee_t **employeeout, char *employeeName, unsigned int count)
{
  int i;
  int newPosition = 0;
  // Go through employees and if it sees employee to remove it just
  // puts employee[i+1]'s info in there and keeps moving
  for (i = 0; i < count; i++)
  {
    if (strcmp((*employeeout)[i].name, employeeName) != 0)
    {
      (*employeeout)[newPosition] = (*employeeout)[i];
      newPosition++;
    }
  }

  return STATUS_SUCCESS;
}

// Prints out the employees in employeeout
// Employees given an id by their index in the array and then their info is printed
void list_employees(struct employee_t **employeeout, unsigned int count)
{
  int i;
  for (i = 0; i < count; i++)
  {
    printf("Employee %d\n", i);
    printf("\tName %s\n", (*employeeout)[i].name);
    printf("\tAddress %s\n", (*employeeout)[i].address);
    printf("\tHours Worked %d\n", (*employeeout)[i].hours);
  }
}