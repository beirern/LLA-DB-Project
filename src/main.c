#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <file.h>
#include <headers.h>
#include <common.h>
#include <employees.h>

// Prints the help/usage message
void print_help(char *argv[])
{
  printf("Usage: %s -f <file-name> [OPTIONS]\n", argv[0]);
  printf("\t-f <file-name> (Required) Name of the file storing the database\n");
  printf("\t-n Whether to create a new file. Fails if file already exists\n");
  printf("\t-q <name> Queries the DB for the employee with the given name. Necessary if looking to remove/update an employee\n");
  printf("\t-e <name>,<address>,<hours> Adds employee with the given info. Info must be comma separated\n");
  printf("\t-l Whether to list the employees in the database\n");
  printf("\t-u <hours> Must be combined with -q! Update's employees hours in DB\n");
  printf("\t-r Must be combined with -q! Removes the employee from the DB\n");
  printf("\t-h Print the help message\n");
  printf("\n");
  printf("Examples\n");
  printf("%s -f employees.db -h\n", argv[0]);
  printf("\tPrint Help Message\n");
  printf("%s -f employees.db -n -e \"Nicola Beirer,123 Lane St,40\"\n", argv[0]);
  printf("\tCreates a database in a new file and adds the employee Nicola Beirer at 123 Lane St that worked 40 hours\n");
  printf("%s -f employees.db q \"Nicola Beirer\" -u 23\n", argv[0]);
  printf("\tUpdates the employee with the name Nicola Beirer to have worked 23 hours\n");
  printf("%s -f employees.db q \"Nicola Beirer\" -r -l\n", argv[0]);
  printf("\tRemoves the employee with the name Nicola Beirer and then lists the employees in the database\n");
  printf("\n");
  printf("Order of operations for flags\n");
  printf("\tSince certain flags can change the database state it is important to consider what order they are evaluated.\n");
  printf("\tThe -h flag will bypass all flags to print the help message and exit.\n");
  printf("\tThe -l operation will always be done last, meaning it will print employee information with changes made with other flags\n");
  printf("\tThe -r operation done after any updates so it is possible to both update and then remove and employee\n");
  printf("\tThe -e operation is done before any database modifications so creating and then querying an employee is possible\n");
  printf("\n");
  printf("Finally, it is possible to have more than 1 employee with the same name. If querying for an employee it will return the employee with the first match\n");
}

// Frees the main pointers that we use
void free_pointers(struct dbheader_t **headerout, struct employee_t **employeeout)
{
  free(*headerout);
  free(headerout);
  free(*employeeout);
  free(employeeout);
}

int main(int argc, char *argv[])
{
  // Whether we create a new file and name of the file
  int newfile = 0;
  char *fileName = NULL;
  // How many hours a new employee has worked
  // Whether we are listing employees
  int listEmployees = 0;
  // Whether we are removing an employee
  int remove = 0;
  // Whether we are adding an employee and their info
  int newEmployee = 0;
  char *employeeInfo = NULL;
  // New employee's fields parsed from employeeInfo
  char *name = NULL;
  char *address = NULL;
  int hours = 0;
  // The hours to update a new
  unsigned int newHours = 0;
  // If we're looking for an employee this is the name to look for
  char *employeeName = NULL;
  // If we want debug output
  int debug = 0;

  // Int for getting the codes passed in
  int getOptCode;

  while ((getOptCode = getopt(argc, argv, "nf:q:e:lu:rhd")) != -1)
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
        return OPTION_PARSING_ERROR;
      }
      address = strtok(NULL, delim);
      if (address == NULL)
      {
        fprintf(stderr, "Failed to parse out the address of the employee!\n");
        fprintf(stderr, "Employee attributes should be comma separated\n");
        return OPTION_PARSING_ERROR;
      }
      char *hoursstr = strtok(NULL, delim);
      if (hoursstr == NULL)
      {
        fprintf(stderr, "Failed to parse out the hours of the employee!\n");
        fprintf(stderr, "Employee attributes should be comma separated\n");
        return OPTION_PARSING_ERROR;
      }
      hours = atoi(hoursstr);
      if (hours == 0)
      {
        perror("Failed to convert the hours of employee to a string!");
        fprintf(stderr, "Employee attributes should be comma separated\n");
        return OPTION_PARSING_ERROR;
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
    case 'h':
      print_help(argv);
      return OPTION_PARSING_ERROR;
    case 'd':
      debug = 1;
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
      return OPTION_PARSING_ERROR;
    default:
      return OPTION_PARSING_ERROR;
    }
  }

  // Need a db file
  if (fileName == NULL)
  {
    fprintf(stderr, "File required! Usage: %s -f <file-name>\n", argv[0]);
    return OPTION_PARSING_ERROR;
  }

  // We need to query for an employee to update or remove their information
  if ((newHours || remove) && !employeeName)
  {
    fprintf(stderr, "If you are trying to update or remove an employee you must pass in their name with -q!\n");
  }

  // The file descriptor for the db file
  int fd;
  // Double pointer so that we can modify and pass reference for what the headers are
  struct dbheader_t **headerout = malloc(sizeof(struct dbheader_t *));

  // Create a file and craft the default headers
  // Otherwise read in a file and read in its headers
  if (newfile)
  {
    fd = create_db_file(fileName);
    if (fd == -1)
    {
      return GENERAL_ERROR;
    }
    if (intialize_default_headers(headerout) != STATUS_SUCCESS)
    {
      free(*headerout);
      free(headerout);
      close(fd);
      return GENERAL_ERROR;
    }
  }
  else
  {
    fd = open_db_file(fileName);
    if (fd == -1)
    {
      return GENERAL_ERROR;
    }

    if (read_headers_from_file(fd, headerout) != STATUS_SUCCESS)
    {
      free(*headerout);
      free(headerout);
      close(fd);
      return GENERAL_ERROR;
    }
  }

  if (debug)
  {
    printf("Initial headers\n");
    print_header(*headerout);
  }

  // Double pointer so that we can modify and pass reference for what the list of employees is
  struct employee_t **employeeout = malloc(sizeof(struct employee_t *));

  // Allocate memory for the number of employees in the DB
  *employeeout = calloc((*headerout)->count, sizeof(struct employee_t));
  if (*employeeout == NULL)
  {
    perror("Failed to allocate memory for employees");
    free_pointers(headerout, employeeout);
    close(fd);
    return GENERAL_ERROR;
  }
  if (read_employees(employeeout, fd, (*headerout)->count) != STATUS_SUCCESS)
  {
    free_pointers(headerout, employeeout);
    close(fd);
    return GENERAL_ERROR;
  }

  // Make a new employee and reallocate the employee array to have enough room for them
  if (newEmployee)
  {
    (*headerout)->count += 1;
    *employeeout = reallocarray(*employeeout, (*headerout)->count, sizeof(struct employee_t));
    if (*employeeout == NULL)
    {
      fprintf(stderr, "Failed to reallocate the memory for employees");
      free_pointers(headerout, employeeout);
      close(fd);
      return GENERAL_ERROR;
    }
    if (add_employee(employeeout, name, address, hours, (*headerout)->count) != STATUS_SUCCESS)
    {
      fprintf(stderr, "Failed to add employee to array");
      free_pointers(headerout, employeeout);
      close(fd);
      return GENERAL_ERROR;
    }
    (*headerout)->filesize += sizeof(struct employee_t);
  }

  // Logic for finding and possibly modifying an employee
  if (employeeName)
  {
    struct employee_t *e = query_employee(employeeout, employeeName, (*headerout)->count);

    if (e == NULL)
    {
      fprintf(stderr, "Failed to find employee\n");
      free_pointers(headerout, employeeout);
      close(fd);
      return EMPLOYEE_NOT_FOUND_ERROR;
    }
    else
    {
      printf("Found %s!\n", e->name);
      if (newHours)
      {
        e->hours = newHours;
        printf("Updated %s's hours to %d\n", e->name, e->hours);
      }

      if (remove)
      {
        if (remove_employee(employeeout, employeeName, (*headerout)->count) != STATUS_SUCCESS)
        {
          fprintf(stderr, "Failed to remove employee %s\n", e->name);
          free_pointers(headerout, employeeout);
          close(fd);
          return GENERAL_ERROR;
        }

        // Update db info about having one less employee
        (*headerout)->count -= 1;
        (*headerout)->filesize -= sizeof(struct employee_t);
        *employeeout = reallocarray(*employeeout, (*headerout)->count, sizeof(struct employee_t));
        // If the count is 0 then pointer can be NULL so skip this check
        if ((*headerout)->count != 0 && *employeeout == NULL)
        {
          fprintf(stderr, "Failed to reallocate the memory for employees");
          free_pointers(headerout, employeeout);
          close(fd);
          return GENERAL_ERROR;
        }
        printf("Removed %s!\n", employeeName);
      }
    }
  }

  // List all the employees in the database
  if (listEmployees)
  {
    list_employees(employeeout, (*headerout)->count);
  }

  // Print the header info if in debug mode
  if (debug)
  {
    printf("Writing the following file\n");
    print_header(*headerout);
  }

  // Write headers and employee information to file
  if (write_to_file(fd, headerout, employeeout) != STATUS_SUCCESS)
  {
    free_pointers(headerout, employeeout);
    close(fd);
    return GENERAL_ERROR;
  }

  // Free all the pointers and close the file
  free_pointers(headerout, employeeout);
  close(fd);

  return 0;
}