#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include "checksum.h"
#include "crc.h"
#include "hammcode.h"
#include "helper_functions.h"
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
    uint16_t crc = 0;
    uint16_t crcSyndrome;
    uint16_t HC = 0;
    uint8_t HCSyndrome;
    uint8_t HCSECDED = 0;
    bool breakCond = false;
    int numErrorDetected = 0;
    int numErrorCorrected = 0;
    int totalValues = 0;
    struct timeval tval_before, tval_after;
    float timeAverage = 0.0;
    while (1)
    {
        gettimeofday(&tval_before, NULL);
        switch (configNumber)
        {
            // See README for configuration details.
            case BIT8_SNGL_PRES_CHECKSUM:
            case BIT8_DBL_PRES_CHECKSUM:
            case BIT8_SNGL_PRES_RES_CHECKSUM:
                if(fread(dwCW, sizeof(uint8_t), 2, fptrCorrCS) == 2)
                {
                    checksumSize = 4;
                    checksum = checksumFuncPtrArr[configNumber](dwCW[0], checksumSize);
                }
                else breakCond = true;
                break;

            case BIT8_HONEYWELL_CHECKSUM:
                // Make a checksum for every two datawords.
                if(fread(dwCW, sizeof(uint8_t), 2, fptrCorrCS) == 2)
                {
                    if (dwCW[0] % 2 == 1)
                    {
                        checksum = checksumFuncPtrArr[configNumber](dwCW[0]-1, dwCW[0]);
                    }
                }
                else breakCond = true;
                break;

            case BIT8_CRC:
                if(fread(&crc, sizeof(uint16_t), 1, fptrCorrCS) == 1)
                {
                    crcSyndrome = CRC4(crc, 12, 0x17);
                    // printf("%d\n", crcSyndrome);
                }
                else breakCond = true;
                break;

            case BIT8_HC_SEC:
                if(fread(&HC, sizeof(uint16_t), 1, fptrCorrCS) == 1)
                {
                    HCSyndrome = HC_12bCW_8bDW_syndrome_SEC(&HC);
                }
                else breakCond = true;
                break;

            case BIT8_HC_SECDED:
                if(fread(&HC, sizeof(uint16_t), 1, fptrCorrCS) == 1)
                {
                    HCSECDED = HC_13bCW_8bDW_syndrome_SECDED(&HC, &HCSyndrome);
                }
                else breakCond = true;
                break;

            default:
                breakCond = true;
                break;
        }

        gettimeofday(&tval_after, NULL);
        float getDiff = timediff_us(tval_before, tval_after);
        timeAverage += getDiff;

        if(breakCond)
        {
            // printf("Breaking out of read loop now\n");
            break;
        }

        switch (configNumber)
        {
            // See README for configuration details.
            case BIT8_SNGL_PRES_CHECKSUM:
            case BIT8_DBL_PRES_CHECKSUM:
            case BIT8_SNGL_PRES_RES_CHECKSUM:
            case BIT8_HONEYWELL_CHECKSUM:
                if (checksum != dwCW[1])
                {
                    numErrorDetected++;
                }
                break;
            case BIT8_CRC:
                if(crcSyndrome) numErrorDetected++;
                else printf("crc codeword not fail: %d\n", crc);
                break;
            case BIT8_HC_SEC:
                if(HCSyndrome)
                {
                    numErrorDetected++;
                    numErrorCorrected++;
                }
                break;
            case BIT8_HC_SECDED:
                if(HCSECDED || HCSyndrome)
                {
                    if(HCSECDED)
                    {
                        numErrorCorrected++;
                        numErrorDetected++;
                    }
                    else
                    {
                        numErrorDetected++;
                    }
                }
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