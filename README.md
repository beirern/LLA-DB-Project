# Database on Disk

This project creates a database that writes the data to disk. The database is meant to hold information about employees, specifically their name, address, and hours worked. Project is from [Low Level Academy's](lowlevel.academy) Zero to Hero C Course. Interaction with the DB is done through command line args.

This is my first project in C.

# Usage
Run `./bin/final -h` for more information!

If you need to build the file run `./build_object_files.sh` which runs some gcc commands for files header and c files in `include` and `src`. I found this easier than trying to make a Makefile.

# Project Specs

## File Validation
* Check that the DB file is using the version of the project API
* Check that the size of the DB file is what we expect, i.e. it should only be the size of the file header's + the size of the employee struct

## DB Interactions
* Read a DB file specifying a list of employees
* Add an employee to the DB
* Update the hours an employee has worked given their name
* Remove an employee from the DB given their name