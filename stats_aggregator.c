#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "helper_functions.h"

int main(int argc, char **argv)
{
    int numIterations;
    if (argc >= 2)
    {
        numIterations = atoi(argv[1]);
    }
    else
    {
        numIterations = DEFAULT_ITERATIONS;
    }

    printf("Start data aggregator.\n");

    FILE *fptrTiming;
    FILE *fptrDetected;
    FILE *fptrCorrected;
    fptrTiming = fopen("data_timing.txt", "r");
    fptrDetected = fopen("data_detected.txt", "r");
    fptrCorrected = fopen("data_corrected.txt", "r");

    float avgTimeEncode = 0.0;
    float avgTimeDecode = 0.0;
    float avgDetected = 0.0;
    float avgCorrected = 0.0;
    
    char line[BUFF_SIZE];
    for (int i = 0; i < numIterations; i++)
    {
        // Encoding and decoding values are in the same file, and they are printed one after another.
        // The pattern is:
        // Encoding val0
        // Decoding val0
        // Encoding val1
        // Encoding val1
        // ... and so forth
        // Get encoding running average.
        if(fgets(line, sizeof(line), fptrTiming) != NULL)
        {
            avgTimeEncode += atof(line);
        }

        // Get decoding running average. 
        if(fgets(line, sizeof(line), fptrTiming) != NULL)
        {
            avgTimeDecode += atof(line);
        }

        // Get detected percentage running average.
        if(fgets(line, sizeof(line), fptrDetected) != NULL)
        {
            avgDetected += atof(line);
        }

        // Get corrected percentage running average.
        if(fgets(line, sizeof(line), fptrCorrected) != NULL)
        {
            avgCorrected += atof(line);
        }
    }

    avgTimeEncode /= (float)numIterations;
    avgTimeDecode /= (float)numIterations;
    avgDetected /= (float)numIterations;
    avgCorrected /= (float)numIterations;

    printf("\n");
    printf("---------- FINAL STATISTICS ----------\n");
    printf("\n");
    printf("Number iterations: %d.\n", numIterations);
    printf("Average time to ENCODE: %f.\n", avgTimeEncode);
    printf("Average time to DECODE: %f.\n", avgTimeDecode);
    printf("Average percent DETECTED corruptions: %f.\n", avgDetected);
    printf("Average percent CORRECTED corruptions: %f.\n", avgCorrected);

    fclose(fptrTiming);
    fclose(fptrDetected);
    fclose(fptrCorrected);
    printf("End data aggregator.\n");
    return 0;

}