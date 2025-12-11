#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "helper_functions.h"
#include "types.h"


int main(int argc, char **argv)
{
    int configNumber;
    int corruptionType;
    int corruptionTypeOption = -1; 
    
    //printf("Number of args: %d.\n", argc);

    // Corruption option is specified.
    if (argc >= 4)
    {
        configNumber = atoi(argv[1]);
        corruptionType = atoi(argv[2]);
        corruptionTypeOption = atoi(argv[3]);
    }
    // Corruption option not specified.
    else if (argc == 3)
    {
        configNumber = atoi(argv[1]);
        corruptionType = atoi(argv[2]);
        
        // Set default length for burst errors.
        if ((corruptionType == CORR_BURST) || (corruptionType == CORR_RAND))
        {
            corruptionTypeOption = 3;
        }
    }
    else if (argc == 2)
    {
        configNumber = atoi(argv[1]);
        corruptionType = CORR_SNGL_BIT;
    }
    else
    {
        configNumber = BIT8_SNGL_PRES_CHECKSUM;
        corruptionType = CORR_SNGL_BIT;
    }
    //printf("ConfigNumber: %d.\n", configNumber);
    //printf("CorruptionType: %d.\n", corruptionType);
    //printf("corruptionTypeOption: %d.\n", corruptionTypeOption);
    
    printf("Start data corruptor.\n");
    FILE *fptrHR;
    FILE *fptrCS;
	
	fptrHR = fopen("data.txt", "a+");
    if (fptrHR == NULL) {
        perror("Error opening fptrHR.\n");
        return 1;
    }

    fptrCS = fopen("data.bin", "rb");
    if (fptrCS == NULL) {
        perror("Error opening fptrCS.\n");
        return 1;
    }

    // Create corrupted files.
    FILE *fptrCorrHR; // Human readable.
    FILE *fptrCorrCS; // Computer storage.
    fptrCorrHR = fopen("data_corrupted.txt", "w");
    fptrCorrCS = fopen("data_corrupted.bin", "wb");
    
    char line[BUFF_SIZE];
    char line_after_space[BUFF_SIZE];
    int counter = 0;
    uint16_t mask;
    uint8_t dwCW[2];
    uint32_t dwCW32[2];
    uint8_t HWChecksumVals[3];
    uint32_t HWChecksumVals32[3];
    uint16_t ResidArithVals[3];
    uint16_t CW;
    uint8_t dataOrCS;
    uint8_t errorIdx;

    // Corrupt the human readable and binary file.
    while (fgets(line, sizeof(line), fptrHR) != NULL)
    {
        mask = calculate_bit_mask(configNumber, corruptionType, corruptionTypeOption);
        // printf("Mask: %d.\n", mask);

        switch(configNumber)
        {
            case BIT8_SNGL_PRES_CHECKSUM:
            case BIT8_DBL_PRES_CHECKSUM:
            case BIT8_SNGL_PRES_RES_CHECKSUM:
                // Get the values as ints.
                get_buffer_after_space(line, line_after_space, BUFF_SIZE);
                dwCW[0] = (uint8_t)atoi(line);
                dwCW[1] = (uint8_t)atoi(line_after_space);
                apply_16_bit_mask(mask, dwCW);
                // Get and print the corrupted values.
                fprintf(fptrCorrHR, "%hhu %hhu\n", dwCW[0], dwCW[1]);
                
                
                // Also do the same for the binary file.
                if (fread(dwCW, sizeof(uint8_t), 2, fptrCS) == 2)
                {
                    //printf("%hhu %hhu\n", dwCW[0], dwCW[1]);
                    apply_16_bit_mask(mask, dwCW);
                    fwrite(&dwCW[0], sizeof(uint8_t), 1, fptrCorrCS);
                    fwrite(&dwCW[1], sizeof(uint8_t), 1, fptrCorrCS);
        
                }
                break;

            case BIT8_HONEYWELL_CHECKSUM:
                human_readable_tokenizer_8b(HWChecksumVals, line);
                errorIdx = rand() % 3;
                HWChecksumVals[errorIdx] ^= mask;
                fprintf(fptrCorrHR, "%hhu %hhu %hhu\n", HWChecksumVals[0], HWChecksumVals[1], HWChecksumVals[2]);
                fwrite(&HWChecksumVals, sizeof(uint8_t), 3, fptrCorrCS);
                break;

            case BIT8_CRC:
            case BIT8_HC_SEC:
            case BIT8_HC_SECDED:
                CW = (uint16_t)atoi(line);
                CW ^= mask;
                fprintf(fptrCorrHR, "%hu\n", CW);
                fwrite(&CW, sizeof(uint16_t), 1, fptrCorrCS);

                // if(fread(&CW, sizeof(uint16_t), 1, fptrCS) == 1)
                // {
                //     CW ^= mask;
                // }
                break;

            case BIT8_RESID_ARITH:
            case BIT8_BIRESID:
                human_readable_tokenizer_16b(ResidArithVals, line);
                ResidArithVals[2] ^= mask;
                fprintf(fptrCorrHR, "%hu %hu %hu\n", ResidArithVals[0], ResidArithVals[1], ResidArithVals[2]);
                fwrite(&ResidArithVals, sizeof(uint16_t), 3, fptrCorrCS);
                break;

            case BIT32_SNGL_PRES_CHECKSUM:
            case BIT32_SNGL_PRES_RES_CHECKSUM:
                get_buffer_after_space(line, line_after_space, BUFF_SIZE);
                dwCW32[0] = (uint32_t)atoi(line);
                dwCW32[1] = (uint32_t)atoi(line_after_space);
                dataOrCS = rand() % 2;
                if(dataOrCS) dwCW32[1] ^= mask;
                else
                {
                    dwCW32[0] ^= bitmask_16b_to_32b(mask);
                }

                fprintf(fptrCorrHR, "%u %hu\n", dwCW32[0], dwCW32[1]);
                fwrite(&dwCW32[0], sizeof(uint32_t), 1, fptrCorrCS);
                fwrite(&dwCW32[1], sizeof(uint16_t), 1, fptrCorrCS);
                break;

            case BIT32_DBL_PRES_CHECKSUM:
                get_buffer_after_space(line, line_after_space, BUFF_SIZE);
                dwCW32[0] = (uint32_t)atoi(line);
                dwCW32[1] = (uint32_t)atoi(line_after_space);
                dataOrCS = rand() % 2;
                dwCW32[dataOrCS] ^= bitmask_16b_to_32b(mask);

                fprintf(fptrCorrHR, "%u %u\n", dwCW32[0], dwCW32[1]);
                fwrite(&dwCW32[0], sizeof(uint32_t), 1, fptrCorrCS);
                fwrite(&dwCW32[1], sizeof(uint32_t), 1, fptrCorrCS);
                break;

            case BIT32_HONEYWELL_CHECKSUM:
                human_readable_tokenizer_32b(HWChecksumVals32, line);
                errorIdx = rand() % 3;
                HWChecksumVals32[errorIdx] ^= bitmask_16b_to_32b(mask);
                fprintf(fptrCorrHR, "%u %u %u\n", HWChecksumVals32[0], HWChecksumVals32[1], HWChecksumVals32[2]);
                fwrite(&HWChecksumVals32, sizeof(uint32_t), 3, fptrCorrCS);
                break;

        }
        
        counter++;
    }

    fclose(fptrCorrHR);
    fclose(fptrHR);
    fclose(fptrCorrCS);
    fclose(fptrCS);

    


    printf("End data corruptor.\n");
    return 0;

}