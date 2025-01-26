#!/bin/bash

### This script builds the executable and if an argument is passed in
### (doesnt really check what the arg is so it is arbitrary) it will be
### built with debug flags. Essentially my verison of a makefile.

set -e

debug_flags=""
# Check if debugging
if [ -n "$1" ]; then
    echo "debug"
    debug_flags="-g "
fi

# Initialize the variable as an empty string
o_files=""

# Compile each .c file
# for file in $(find ./src -type f); do
for file in ./src/*.c; do
    base_file=$(basename "$file" .c)
    echo "gcc $debug_flags -c $file -o obj/$base_file.o -Iinclude"
    gcc $debug_flags -c "$file" -o "obj/$base_file.o" -Iinclude

    # Append to the variable
    o_files+="obj/$base_file.o "
done

echo "gcc $debug_flags -o bin/final $o_files"
gcc $debug_flags -o bin/final $o_files
