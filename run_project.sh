#!/bin/bash

echo "Building and running the project."

# Cleaning project.
chmod +x ./clean_project.sh
./clean_project.sh

chmod +x ./clean_project_data.sh

# Compile project.
echo "Compile files."
gcc data_gen.c -O3 -o data_gen.exe
gcc data_corruptor.c -O3 -o data_corruptor.exe
gcc data_read.c -O3 -o data_read.exe
gcc stats_aggregator.c -o stats_aggregator.exe

# Run all programs.
# $1 represents the configurationNumber.
# $2 represents corruptionType.
# $3 represents corruptionTypeOption

echo "----- START RUN -----"
echo ""

# Run base case first
for h in $(seq 1 100)
do
    ./data_gen.exe $1
    ./data_corruptor.exe $1 0
    ./data_read.exe $1
done

# Run the statistics aggregator to average the statistics from all the runs.
./stats_aggregator.exe 100 $1 0

./clean_project_data.sh

for i in $(seq 1 2)
do
    for j in $(seq 2 $2)
    do
        for h in $(seq 1 100)
        do
            ./data_gen.exe $1
            ./data_corruptor.exe $1 $i $j
            ./data_read.exe $1
        done

        # Run the statistics aggregator to average the statistics from all the runs.
        ./stats_aggregator.exe 100 $1 $i $j
        ./clean_project_data.sh
    done
done

echo ""
echo "----- END RUN -----"
