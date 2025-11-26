#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include "checksum.h"
#include "crc.h"

#define BITS_PER_BYTE  8
#define BYTES_PER_CHAR 1
#define NUM_CHAR       1  
#define SIZE_DW_BITS (BITS_PER_BYTE*BYTES_PER_CHAR*NUM_CHAR)
#define MAX_DW_VALUE (1 << SIZE_DW_BITS)-1


int main(int argc, char **argv)
{

    int configNumber;
    if (argc == 2)
    {
        configNumber = atoi(argv[1]);
    }
    else
    {
        configNumber = 1;
    }

    FILE *fptrHR; // Human readable.
    FILE *fptrCS; // Computer storage.

	
	fptrHR = fopen("data.txt", "w");
    fptrCS = fopen("data.bin", "wb");

    uint16_t counter = 0;
    uint8_t keepGoing = 1;
    uint8_t checksum;
    uint8_t checksumSize;

    // Variables for keeping track of time.
    volatile clock_t start_tick;
    volatile double averageTime = 0.0;
    while (keepGoing == 1)
    {

        if (counter >= MAX_DW_VALUE)
        {
            keepGoing = 0;
            break;
        }

        start_tick = clock();
        switch (configNumber)
        {

            // See README for configuration details.s
            case 1:
                checksumSize = 4;
                checksum = checksum_4bitCW_8bDW_snglPrec(counter, checksumSize);
                break;
            case 2:
                checksumSize = 4;
                checksum = checksum_4bitCW_8bDW_doubPrec(counter, checksumSize);
                break;
            case 3:
                checksumSize = 4;
                checksum = residueChecksum_4bitCW_8bDW_snglPrec(counter, checksumSize);
                break;
            case 4:
                // Make a checksum for every two datawords.
                if (counter % 2 == 1)
                {
                    checksum = HWChecksum_4bitCW_8bDW (counter-1, counter);
                }
                break;
            default:
                break;
        }
        // Add to the running average.
        printf("Time to compute code iteration %d: %f.\n", counter, (((double) (clock() - start_tick)) / CLOCKS_PER_SEC));
        averageTime += (((double) (clock() - start_tick)) / CLOCKS_PER_SEC);

        switch (configNumber)
        {
            case 1:
            case 2:
            case 3:
                fprintf(fptrHR, "%hhu %hhu\n", counter, checksum);
                fwrite(&counter, sizeof(uint8_t), 1, fptrCS);
                fwrite(&checksum, sizeof(uint8_t), 1, fptrCS);
                break;
            case 4:
                // Only write the checksum every two datawords.
                if (counter % 2 == 1)
                {
                    fprintf(fptrHR, "%hhu %hhu\n", counter, checksum);
                    fwrite(&counter, sizeof(uint8_t), 1, fptrCS);
                    fwrite(&checksum, sizeof(uint8_t), 1, fptrCS);
                }
                else
                {
                    fprintf(fptrHR, "%hhu\n", counter);
                    fwrite(&counter, sizeof(uint8_t), 1, fptrCS);
                }
                break;
            default:
                break;

        }
        
        // fprintf(fptrHR, "%hhu %hhu %hhu\n", counter, counter+1, checksum);
        
        counter++;
    }

    // Find the average by dividing by number of iterrations.
    averageTime /= MAX_DW_VALUE+1;

    printf("The average time to compute the codeword is: %f.\n", averageTime);

    fclose(fptrHR);
    
    return 0;

}