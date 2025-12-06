#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include "types.h"
#include "checksum.h"
#include "crc.h"
#include "helper_functions.h"

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

    printf("Data read.\n");

    FILE *fptrCorrCS;
    fptrCorrCS = fopen("data_corrupted.bin", "rb");

    uint8_t (*checksumFuncPtrArr[4])(uint8_t, uint8_t) = 
    {
        checksum_4bitCW_8bDW_snglPrec,
        checksum_4bitCW_8bDW_doubPrec,
        residueChecksum_4bitCW_8bDW_snglPrec,
        HWChecksum_4bitCW_8bDW
    };

    // Read through the corrupted file.
    uint8_t dwCW[2];
    uint8_t checksumSize;
    uint8_t checksum;
    uint16_t crc;
    uint16_t crcSyndrome;
    bool breakCond = false;
    int numErrorDetected = 0;
    int numErrorCorrected = 0;
    int totalValues = 0;
    struct timeval tval_before, tval_after;
    float timeAverage = 0.0;
    while (1)
    {
        switch (configNumber)
        {
            // See README for configuration details.
            case BIT8_SNGL_PRES_CHECKSUM:
            case BIT8_DBL_PRES_CHECKSUM:
            case BIT8_SNGL_PRES_RES_CHECKSUM:
                if(fread(dwCW, sizeof(uint8_t), 2, fptrCorrCS) == 2)
                {
                    gettimeofday(&tval_before, NULL);
                    checksumSize = 4;
                    checksum = checksumFuncPtrArr[configNumber](dwCW[0], checksumSize);
                    // Add to the running average.
                    gettimeofday(&tval_after, NULL);
                    float getDiff = timediff_us(tval_before, tval_after);
                    // Calculate the elapsed time microseconds.
                    timeAverage += getDiff;
                }
                else breakCond = true;
                break;
            case BIT8_HONEYWELL_CHECKSUM:
                // Make a checksum for every two datawords.
                if(fread(dwCW, sizeof(uint8_t), 2, fptrCorrCS) == 2)
                {
                    if (dwCW[0] % 2 == 1)
                    {
                        gettimeofday(&tval_before, NULL);
                        checksum = checksumFuncPtrArr[configNumber](dwCW[0]-1, dwCW[0]);
                        // Add to the running average.
                        gettimeofday(&tval_after, NULL);
                        float getDiff = timediff_us(tval_before, tval_after);
                        // Calculate the elapsed time microseconds.
                        timeAverage += getDiff;
                    }
                }
                else breakCond = true;
                break;
            case BIT8_CRC:
                if(fread(&crc, sizeof(uint16_t), 1, fptrCorrCS) == 1)
                {
                    gettimeofday(&tval_before, NULL);
                    crcSyndrome = CRC4_12bCW_8bDW(crc, 12, 0x17);
                    // printf("%d\n", crcSyndrome);
                    // Add to the running average.
                    gettimeofday(&tval_after, NULL);
                    float getDiff = timediff_us(tval_before, tval_after);
                    // Calculate the elapsed time microseconds.
                    timeAverage += getDiff;
                }
                else breakCond = true;
            default:
                break;
        }

        if(breakCond) break;

        switch (configNumber)
        {
            // See README for configuration details.
            case BIT8_SNGL_PRES_CHECKSUM:
            case BIT8_DBL_PRES_CHECKSUM:
            case BIT8_SNGL_PRES_RES_CHECKSUM:
            case BIT8_HONEYWELL_CHECKSUM:
                if (checksum != dwCW[0])
                {
                    numErrorDetected++;
                }
                break;
            case BIT8_CRC: 
                if(crcSyndrome) numErrorDetected++;
            default:
                break;
        }

        totalValues++;
    }

    // Find the average by dividing by number of iterrations.
    timeAverage /= (float)(totalValues);

    printf("The average time to decode the codeword is: %f microseconds.\n", timeAverage);
    printf("Number of errors detected: %d.\n", numErrorDetected);
    printf("Total corrupted values: %d.\n", totalValues);
    printf("Percent errors detected: %f.\n", (float)numErrorDetected/(float)totalValues);
    fclose(fptrCorrCS);
    printf("End data read.\n");
}