#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
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

    //printf("Data read.\n");

    FILE *fptrCorrCS;
    // Get file pointers for statistics.
    FILE *fptrTiming;
    FILE *fptrDetected;
    FILE *fptrCorrected;

    fptrTiming = fopen("data_timing.txt", "a");
    fptrDetected = fopen("data_detected.txt", "a");
    fptrCorrected = fopen("data_corrected.txt", "a");
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
    uint8_t HWChecksumVals[3];
    uint16_t crc = 0;
    uint16_t crcSyndrome;
    uint16_t HC = 0;
    uint8_t HCSyndrome;
    uint8_t HCSECDED = 0;
    uint16_t ResidArithVals[3];
    bool residCodeCheck = false;

    uint32_t dwCW32[2];
    uint32_t checksum32;

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
                    checksumSize = 4;
                    gettimeofday(&tval_before, NULL);
                    checksum = checksumFuncPtrArr[configNumber](dwCW[0], checksumSize);
                    gettimeofday(&tval_after, NULL);
                }
                else breakCond = true;
                break;

            case BIT8_HONEYWELL_CHECKSUM:
                // Make a checksum for every two datawords.
                if(fread(HWChecksumVals, sizeof(uint8_t), 3, fptrCorrCS) == 3)
                {
                    gettimeofday(&tval_before, NULL);
                    checksum = checksumFuncPtrArr[configNumber](HWChecksumVals[0], HWChecksumVals[1]);
                    gettimeofday(&tval_after, NULL);
                }
                else breakCond = true;
                break;

            case BIT8_CRC:
                if(fread(&crc, sizeof(uint16_t), 1, fptrCorrCS) == 1)
                {
                    gettimeofday(&tval_before, NULL);
                    crcSyndrome = CRC4_decode (crc, 12, 0x17);
                    // printf("%d\n", crcSyndrome);
                    gettimeofday(&tval_after, NULL);
                }
                else breakCond = true;
                break;

            case BIT8_HC_SEC:
                if(fread(&HC, sizeof(uint16_t), 1, fptrCorrCS) == 1)
                {
                    gettimeofday(&tval_before, NULL);
                    HCSyndrome = HC_12bCW_8bDW_syndrome_SEC(&HC);
                    gettimeofday(&tval_after, NULL);
                }
                else breakCond = true;
                break;

            case BIT8_HC_SECDED:
                if(fread(&HC, sizeof(uint16_t), 1, fptrCorrCS) == 1)
                {
                    gettimeofday(&tval_before, NULL);
                    HCSECDED = HC_13bCW_8bDW_syndrome_SECDED(&HC, &HCSyndrome);
                    gettimeofday(&tval_after, NULL);
                }
                else breakCond = true;
                break;

            case BIT8_RESID_ARITH:
                if(fread(&ResidArithVals, sizeof(uint16_t), 3, fptrCorrCS) == 3)
                {
                    gettimeofday(&tval_before, NULL);
                    residCodeCheck = lcresidarith_extract_compare(ResidArithVals[0], ResidArithVals[1], ResidArithVals[2], 3);
                    gettimeofday(&tval_after, NULL);
                }
                else breakCond = true;
                break;

            case BIT8_BIRESID:
                if(fread(&ResidArithVals, sizeof(uint16_t), 3, fptrCorrCS) == 3)
                {
                    gettimeofday(&tval_before, NULL);
                    residCodeCheck = biresidue_compare_correct(ResidArithVals[0], ResidArithVals[1], ResidArithVals[2]);
                    gettimeofday(&tval_after, NULL);
                }
                else breakCond = true;
                break;

            case BIT32_SNGL_PRES_CHECKSUM:
                if(fread(dwCW32, sizeof(uint32_t), 2, fptrCorrCS) == 2)
                {
                    checksumSize = 16;
                    gettimeofday(&tval_before, NULL);
                    checksum32 = checksum_16bitCW_32bDW_snglPrec(dwCW32[0], checksumSize);
                    gettimeofday(&tval_after, NULL);
                }
                else breakCond = true;
                break;

            case BIT32_DBL_PRES_CHECKSUM:
                if(fread(dwCW32, sizeof(uint32_t), 2, fptrCorrCS) == 2)
                {
                    checksumSize = 16;
                    gettimeofday(&tval_before, NULL);
                    checksum32 = checksum_16bitCW_32bDW_doubPrec(dwCW32[0], checksumSize);
                    gettimeofday(&tval_after, NULL);
                }
                else breakCond = true;
                break;

            case BIT32_SNGL_PRES_RES_CHECKSUM:
                if(fread(dwCW32, sizeof(uint32_t), 2, fptrCorrCS) == 2)
                {
                    checksumSize = 16;
                    gettimeofday(&tval_before, NULL);
                    checksum32 = residueChecksum_16bitCW_32bDW_snglPrec(dwCW32[0], checksumSize);
                    gettimeofday(&tval_after, NULL);
                }
                else breakCond = true;
                break;

            default:
                breakCond = true;
                break;
        }

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
                if (checksum != dwCW[1])
                {
                    numErrorDetected++;
                }
                break;

            case BIT8_HONEYWELL_CHECKSUM:
                if(checksum != HWChecksumVals[2]) numErrorDetected++;
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
                break;

            case BIT8_RESID_ARITH:
                if(!residCodeCheck) numErrorDetected++;
                break;
            
            case BIT8_BIRESID:
                if(residCodeCheck)
                {
                    numErrorDetected++;
                    numErrorCorrected++;
                }
                break;

            case BIT32_SNGL_PRES_CHECKSUM:
            case BIT32_DBL_PRES_CHECKSUM:
            case BIT32_SNGL_PRES_RES_CHECKSUM:
                if(checksum32 != dwCW32[1])
                {
                    numErrorDetected++;
                }

            default:
                break;
        }

        totalValues++;
    }

    // Find the average by dividing by number of iterrations.
    timeAverage /= (float)(totalValues);

    //printf("The average time to decode the codeword is: %f microseconds.\n", timeAverage);
    //printf("Number of errors detected: %d.\n", numErrorDetected);
    //printf("Total corrupted values: %d.\n", totalValues);
    //printf("Percent errors detected: %f.\n", (float)numErrorDetected/(float)totalValues);
    //printf("Percent errors corrcted: %f.\n", (float)numErrorCorrected/(float)totalValues);
    // print to statistics files.
    fprintf(fptrTiming, "%f\n", timeAverage);
    fprintf(fptrDetected, "%f\n", (float)numErrorDetected/(float)totalValues);
    fprintf(fptrCorrected, "%f\n", (float)numErrorCorrected/(float)totalValues);

    fclose(fptrCorrCS);
    fclose(fptrTiming);
    fclose(fptrDetected);
    fclose(fptrCorrected);
    //printf("End data read.\n");
}