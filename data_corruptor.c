#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "helper_functions.h"

#define BUFF_SIZE 100

int main(int argc, char **argv)
{
    int configNumber;
    int corruptionType;
    int corruptionTypeOption = -1; 
    
    // Corruption option is specified.
    if (argc == 4)
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
        if (corruptionType == CORR_BURST)
        {
            corruptionTypeOption = 3;
        }
    }
    else
    {
        configNumber = 1;
        corruptionType = 1;
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

    // Corrupt the human readable and binary file.
    while (fgets(line, sizeof(line), fptrHR) != NULL)
    {
        // Get the values as ints.
        get_buffer_after_space(line, line_after_space, BUFF_SIZE);
        dwCW[0] = (uint8_t)atoi(line);
        dwCW[1] = (uint8_t)atoi(line_after_space);

        // Get and print the corrupted values.
        mask = calculate_16bit_mask(configNumber, corruptionType, corruptionTypeOption);
        //printf("Mask: %d.\n", mask);
        apply_16_bit_mask(mask, dwCW);

        fprintf(fptrCorrHR, "%hhu %hhu\n", dwCW[0], dwCW[1]);


        // Also do the same for the binary file.
        if (fread(dwCW, sizeof(uint8_t), 2, fptrCS) == 2)
        {
            //printf("%hhu %hhu\n", dwCW[0], dwCW[1]);
            apply_16_bit_mask(mask, dwCW);
            fwrite(&dwCW[0], sizeof(uint8_t), 1, fptrCorrCS);
            fwrite(&dwCW[1], sizeof(uint8_t), 1, fptrCorrCS);

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