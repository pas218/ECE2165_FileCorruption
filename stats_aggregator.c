#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "helper_functions.h"
#include "types.h"

int main(int argc, char **argv)
{
    int numIterations;
    int configNumber;
    int corruptionType;
    int corruptionTypeOption = -1;
    if (argc >= 5)
    {
        numIterations = atoi(argv[1]);
        configNumber = atoi(argv[2]);
        corruptionType = atoi(argv[3]);
        corruptionTypeOption = atoi(argv[4]);

    }
    else if (argc == 4)
    {
        numIterations = atoi(argv[1]);
        configNumber = atoi(argv[2]);
        corruptionType = atoi(argv[3]);
        
        // Set default length for burst errors.
        if ((corruptionType == CORR_BURST) || (corruptionType == CORR_RAND))
        {
            corruptionTypeOption = 3;
        }
    }
    else
    {
        numIterations = DEFAULT_ITERATIONS;
        configNumber = BIT8_SNGL_PRES_CHECKSUM;
        corruptionType = CORR_SNGL_BIT;
        corruptionTypeOption = 3;
    }

    //printf("Start data aggregator.\n");

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
    printf("Config number: %d.\n", configNumber);
    printf("Corruption Type: %d.\n", corruptionType);
    printf("Corruption Type Option: %d.\n", corruptionTypeOption);
    printf("Number iterations: %d.\n", numIterations);
    printf("Average time to ENCODE: %f.\n", avgTimeEncode);
    printf("Average time to DECODE: %f.\n", avgTimeDecode);
    printf("Average percent DETECTED corruptions: %f.\n", avgDetected);
    printf("Average percent CORRECTED corruptions: %f.\n", avgCorrected);
    printf("Number of lines tested: %d.\n", MAX_DW_VALUE+1);

    fclose(fptrTiming);
    fclose(fptrDetected);
    fclose(fptrCorrected);
    //printf("End data aggregator.\n");
    return 0;

}