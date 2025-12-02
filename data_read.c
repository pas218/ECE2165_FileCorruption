#include <stdio.h>
#include <stdint.h>
#include "types.h"
#include "checksum.h"
#include "crc.h"
#include "helper_functions.h"

int main(int argc, char **argv)
{

    int configNumber;
    if (argc == 2)
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

    // Read through the corrupted file.
    uint8_t dwCW[2];
    uint8_t checksumSize;
    uint8_t checksum;
    int numErrorDetected = 0;
    int numErrorCorrected = 0;
    int totalValues = 0;
    while (fread(dwCW, sizeof(uint8_t), 2, fptrCorrCS) == 2)
    {
        switch (configNumber)
        {
            // See README for configuration details.
            case BIT8_SNGL_PRES_CHECKSUM:
                checksumSize = 4;
                checksum = checksum_4bitCW_8bDW_snglPrec(dwCW[0], checksumSize);
                break;
            case BIT8_DBL_PRES_CHECKSUM:
                checksumSize = 4;
                checksum = checksum_4bitCW_8bDW_doubPrec(dwCW[0], checksumSize);
                break;
            case BIT8_SNGL_PRES_RES_CHECKSUM:
                checksumSize = 4;
                checksum = residueChecksum_4bitCW_8bDW_snglPrec(dwCW[0], checksumSize);
                break;
            case BIT8_HONEYWELL_CHECKSUM:
                // Make a checksum for every two datawords.
                if (dwCW[0] % 2 == 1)
                {
                    checksum = HWChecksum_4bitCW_8bDW (dwCW[0]-1, dwCW[0]);
                }
                break;
            default:
                break;
        }

        if (checksum != dwCW[0])
        {
            numErrorDetected++;
        }

        totalValues++;
    }

    printf("Number of errors detected: %d.\n", numErrorDetected);
    printf("Total corrupted values: %d.\n", totalValues);
    printf("Percent errors detected: %f.\n", (float)numErrorDetected/(float)totalValues);
    fclose(fptrCorrCS);
}