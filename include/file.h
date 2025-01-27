#ifndef FILE_H
#define FILE_H

#include <headers.h>
#include <employees.h>

int open_db_file(char *fileName);
int create_db_file(char *fileName);
int write_to_file(int fd, struct dbheader_t **headerout, struct employee_t **employeeout);

#endif