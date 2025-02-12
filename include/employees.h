#ifndef EMPLOYEES_H
#define EMPLOYEES_H

struct employee_t
{
  char name[256];
  char address[256];
  unsigned int hours;
};

int read_employees(struct employee_t **employeeout, int fd, unsigned int count);
int add_employee(struct employee_t **employeeout, char *name, char *address, int hours, unsigned int count);
struct employee_t *query_employee(struct employee_t **employeeout, char *name, unsigned int count);
int remove_employee(struct employee_t **employeeout, char *employeeName, unsigned int count);
void list_employees(struct employee_t **employeeout, unsigned int count);

#endif