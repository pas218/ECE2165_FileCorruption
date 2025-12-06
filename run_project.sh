 #!/bin/bash

 echo "Building and running the project."

# Cleaning project.
chmod +x ./clean_project.sh
./clean_project.sh

# Compile project.
 gcc data_gen.c -O3 -o data_gen.exe
 gcc data_corruptor.c -O3 -o data_corruptor.exe
 gcc data_read.c -O3 -o data_read.exe

# Run all programs.
# $1 represents the config number.
 ./data_gen.exe $1
 ./data_corruptor.exe $1 $2 $3
 ./data_read.exe $1