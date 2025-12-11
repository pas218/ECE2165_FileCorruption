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
#include "residuecodes.h"
#include "types.h"


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
    // Files for design statistics.
    FILE *fptrTiming;
    
    

	fptrHR = fopen("data.txt", "w");
    fptrCS = fopen("data.bin", "wb");
    fptrTiming = fopen("data_timing.txt", "a");

    uint16_t counter = 0;
    uint8_t checksum;
    uint8_t checksumSize = 4;
    uint16_t CRC;
    uint16_t HCcodeword = 0;
    uint16_t residArith[3];

    uint32_t currEntry;
    uint16_t checksum32;
    uint32_t checksum32doub;
    uint32_t checksum32size = 16;

    // float get_elapsed_ms(struct timespec start, struct timespec end) {
    // Variables for keeping track of time.
    struct timeval tval_before, tval_after;
    float timeAverage = 0.0;
    while (1)
    {
        switch (configNumber)
        {
            // See README for configuration details.s
            case BIT8_SNGL_PRES_CHECKSUM:
                gettimeofday(&tval_before, NULL);
                checksum = checksum_4bitCW_8bDW_snglPrec(counter, checksumSize);
                gettimeofday(&tval_after, NULL);
                break;

            case BIT8_DBL_PRES_CHECKSUM:
                gettimeofday(&tval_before, NULL);
                checksum = checksum_4bitCW_8bDW_doubPrec(counter, checksumSize);
                gettimeofday(&tval_after, NULL);
                break;

            case BIT8_SNGL_PRES_RES_CHECKSUM:
                gettimeofday(&tval_before, NULL);
                checksum = residueChecksum_4bitCW_8bDW_snglPrec(counter, checksumSize);
                gettimeofday(&tval_after, NULL);
                break;

            case BIT8_HONEYWELL_CHECKSUM:
                gettimeofday(&tval_before, NULL);
                checksum = HWChecksum_4bitCW_8bDW (counter-1, counter);
                gettimeofday(&tval_after, NULL);
                break;

            case BIT8_CRC:
                gettimeofday(&tval_before, NULL);
                CRC = CRC4_encode (counter, 8, 0x17); // defaulted generator polynomial
                // printf("0x%x: 0x%x.\n", counter, CRC);
                gettimeofday(&tval_after, NULL);
                break;

            case BIT8_HC_SEC:
                gettimeofday(&tval_before, NULL);
                HCcodeword = HC_12bCW_8bDW(counter);
                // printf("0x%x: 0x%x.\n", counter, HCcodeword);
                gettimeofday(&tval_after, NULL);
                break;

            case BIT8_HC_SECDED:
                gettimeofday(&tval_before, NULL);
                HCcodeword = HC_13bCW_8bDW(counter);
                gettimeofday(&tval_after, NULL);
                break;

            case BIT8_RESID_ARITH:
                gettimeofday(&tval_before, NULL);
                residArith[0] = lowcost_residuearith_8bDW(counter-1, 3);
                residArith[1] = lowcost_residuearith_8bDW(counter, 3);
                residArith[2] = (counter + (uint8_t)(counter-1));
                gettimeofday(&tval_after, NULL);
                break;

            case BIT8_BIRESID:
                gettimeofday(&tval_before, NULL);
                residArith[0] = biresidue_correction_8bDW_12bCW(counter-1);
                residArith[1] = biresidue_correction_8bDW_12bCW(counter);
                residArith[2] = (counter + (uint8_t)(counter-1));
                gettimeofday(&tval_after, NULL);
                break;

            case BIT32_SNGL_PRES_CHECKSUM:
                currEntry = rand32();
                gettimeofday(&tval_before, NULL);
                checksum32 = checksum_16bitCW_32bDW_snglPrec(currEntry, checksum32size);
                gettimeofday(&tval_after, NULL);
                break;

            case BIT32_DBL_PRES_CHECKSUM:
                currEntry = rand32();
                gettimeofday(&tval_before, NULL);
                checksum32doub = checksum_16bitCW_32bDW_doubPrec(currEntry, checksum32size);
                gettimeofday(&tval_after, NULL);
                break;
                
            default:
                break;
        }
        
        
        // Add to the running average.
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

            case BIT8_RESID_ARITH:
            case BIT8_BIRESID:
                fprintf(fptrHR, "%hu %hu %hu\n", residArith[0], residArith[1], residArith[2]);
                fwrite(&residArith, sizeof(uint16_t), 3, fptrCS);
                break;

            case BIT32_SNGL_PRES_CHECKSUM:
            case BIT32_SNGL_PRES_RES_CHECKSUM:
                fprintf(fptrHR, "%u %u\n", currEntry, checksum32);
                fwrite(&currEntry, sizeof(uint32_t), 1, fptrCS);
                fwrite(&checksum32, sizeof(uint16_t), 1, fptrCS);
                break;
            case BIT32_DBL_PRES_CHECKSUM:
                fprintf(fptrHR, "%u %u\n", currEntry, checksum32doub);
                fwrite(&currEntry, sizeof(uint32_t), 1, fptrCS);
                fwrite(&checksum32doub, sizeof(uint32_t), 1, fptrCS);
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
    // Print the time to encoder to file.
    fprintf(fptrTiming, "%f\n", timeAverage);

    fclose(fptrHR);
    fclose(fptrCS);
    fclose(fptrTiming);
    return 0;

}