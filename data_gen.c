#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h> 
#include "checksum.h"
#include "crc.h"
#include "hammcode.h"
#include "helper_functions.h"
#include "types.h"

#define BITS_PER_BYTE  8
#define BYTES_PER_CHAR 1
#define NUM_CHAR       1  
#define SIZE_DW_BITS (BITS_PER_BYTE*BYTES_PER_CHAR*NUM_CHAR)
#define MAX_DW_VALUE (1 << SIZE_DW_BITS)-1


int main(int argc, char **argv)
{

    int configNumber;
    if (argc >= 2)
    {
        configNumber = atoi(argv[1]);
    }
    else
    {
        configNumber = BIT8_SNGL_PRES_CHECKSUM;
    }

    FILE *fptrHR; // Human readable.
    FILE *fptrCS; // Computer storage.

	
	fptrHR = fopen("data.txt", "w");
    fptrCS = fopen("data.bin", "wb");

    uint16_t counter = 0;
    uint8_t checksum;
    uint8_t checksumSize;
    uint16_t CRC;
    uint16_t HCcodeword = 0;

    // float get_elapsed_ms(struct timespec start, struct timespec end) {
    // Variables for keeping track of time.
    struct timeval tval_before, tval_after;
    float timeAverage = 0.0;
    while (1)
    {
        gettimeofday(&tval_before, NULL);
        switch (configNumber)
        {

            // See README for configuration details.s
            case BIT8_SNGL_PRES_CHECKSUM:
                checksumSize = 4;
                checksum = checksum_4bitCW_8bDW_snglPrec(counter, checksumSize);
                break;

            case BIT8_DBL_PRES_CHECKSUM:
                checksumSize = 4;
                checksum = checksum_4bitCW_8bDW_doubPrec(counter, checksumSize);
                break;

            case BIT8_SNGL_PRES_RES_CHECKSUM:
                checksumSize = 4;
                checksum = residueChecksum_4bitCW_8bDW_snglPrec(counter, checksumSize);
                break;

            case BIT8_HONEYWELL_CHECKSUM:
                checksum = HWChecksum_4bitCW_8bDW (counter-1, counter);
                break;

            case BIT8_CRC:
                CRC = CRC4 (counter, 8, 0x17); // defaulted generator polynomial
                // printf("0x%x: 0x%x.\n", counter, CRC);
                break;

            case BIT8_HC_SEC:
                HCcodeword = HC_12bCW_8bDW(counter);
                // printf("0x%x: 0x%x.\n", counter, HCcodeword);
                break;

            case BIT8_HC_SECDED:
                HCcodeword = HC_13bCW_8bDW(counter);
                break;
                
            default:
                break;
        }
        
        
        // Add to the running average.
        gettimeofday(&tval_after, NULL);
        float getDiff = timediff_us(tval_before, tval_after);
        // Calculate the elapsed time microseconds.
        timeAverage += getDiff;

        switch (configNumber)
        {
            case BIT8_SNGL_PRES_CHECKSUM:
            case BIT8_DBL_PRES_CHECKSUM:
            case BIT8_SNGL_PRES_RES_CHECKSUM:
                fprintf(fptrHR, "%hhu %hhu\n", counter, checksum);
                fwrite(&counter, sizeof(uint8_t), 1, fptrCS);
                fwrite(&checksum, sizeof(uint8_t), 1, fptrCS);
                break;

            case BIT8_HONEYWELL_CHECKSUM:
                // Only write the checksum every two datawords.
                fprintf(fptrHR, "%hhu %hhu %hhu\n", counter-1, counter, checksum);
                uint8_t prevCnt = counter-1;
                fwrite(&prevCnt, sizeof(uint8_t), 1, fptrCS);
                fwrite(&counter, sizeof(uint8_t), 1, fptrCS);
                fwrite(&checksum, sizeof(uint8_t), 1, fptrCS);
                break;

            case BIT8_CRC:
                fprintf(fptrHR, "%hu\n", CRC);
                fwrite(&CRC, sizeof(uint16_t), 1, fptrCS);
                break;

            case BIT8_HC_SEC:
            case BIT8_HC_SECDED:
                // printf("%x\n", HCcodeword);
                fprintf(fptrHR, "%hu\n", HCcodeword);
                fwrite(&HCcodeword, sizeof(uint16_t), 1, fptrCS);
                break;

            default:
                break;

        }
        
        // fprintf(fptrHR, "%hhu %hhu %hhu\n", counter, counter+1, checksum);
        
        counter++;

        if (counter > MAX_DW_VALUE)
        {
            break;
        }
    }

    // Find the average by dividing by number of iterrations.
    timeAverage /= (float)(MAX_DW_VALUE+1);

    printf("The average time to encode the codeword is: %f microseconds.\n", timeAverage);

    fclose(fptrHR);
    
    return 0;

}