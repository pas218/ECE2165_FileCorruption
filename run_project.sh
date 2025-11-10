 #!/bin/bash

 echo "Building and running the project."

# Cleaning project
chmod +x ./clean_project.sh
./clean_project.sh

# Compile project
 gcc data_gen.c -o data_gen.exe
 gcc data_corruptor.c -o data_corruptor.exe
 gcc data_read.c -o data_read.exe

# Run all programs
 ./data_gen.exe
 ./data_corruptor.exe
 ./data_read.exe