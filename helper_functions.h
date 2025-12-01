#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#define NUM_ACCPET_VALUES 10

void convert_uint16_t_to_bit_string(uint16_t input, char helper[])
{
    //char returnVal[16] = "0000000000000000";
    //helper = "0000000000000000";
    // Need to go through 16 bits.
    for (int i = 15; i >= 0; i--)
    {
        if(((0x1<<i)&input) != 0)
        {
            helper[15-i] = '1';
        }
    }

    //printf("ReturnVal: %s.\n", returnVal);
    //helper = returnVal;
}
void adjust_for_12_bits(uint16_t *input)
{
    uint16_t top = (*input & 0xFF00);
    uint16_t bottom = (*input & 0x00FF);
    uint16_t intermediate;
    top = top << 4;
    intermediate = (bottom & 0x00F0) << 4;
    bottom = bottom & 0x000F;
    top = top | intermediate;
    *input = top | bottom;
}

uint16_t build_burst_error_16bit(int length)
{
    uint16_t returnVal = 0;
    for (int i = 0; i < length; i++)
    {
        returnVal = returnVal << 1;
        returnVal += 1;
    }
    
    return returnVal;
}

void get_raw_mask_16bit(int wordSize, int corruptionType, int corruptionTypeOption, uint16_t* returnVal)
{
    //printf("Getting raw mask.\n");
    switch(corruptionType)
    {
        int position;
        case CORR_SNGL_BIT:
            position = rand() % wordSize;
            //printf("Option 1 position: %d.\n", position);
            *returnVal = 0x1 << position;
            break;
        case CORR_BURST:
            int possiblePositions = wordSize-corruptionTypeOption+1;
            position = rand() % possiblePositions;
            //printf("Option 2 position: %d.\n", position);
            *returnVal = build_burst_error_16bit(corruptionTypeOption);
            *returnVal = *returnVal << position;
            break;
        case CORR_RAND:
            // Go through every possible position and decide whether it will be one or zero randomly;
            *returnVal = 0;
            int decider;
            for (int i = 0; i < wordSize; i++)
            {
                decider = rand() % 2;
                if (decider == 1)
                {
                    *returnVal = *returnVal | (0x1 << i);
                }
            }
            break;
    }
}

uint16_t calculate_16bit_mask(int configNumber, int corruptionType, int corruptionTypeOption)
{
    uint16_t returnVal = 0;
    int wordSize;
    //printf("Calculating mask.\n");
    //printf("Start returnVal: %d.\n", returnVal);
    switch(configNumber)
    {
        // These all follow the format of 8 bit data then 4 bit code. However, both data and code use 8 bit numbers.
        case BIT8_SNGL_PRES_CHECKSUM:
        case BIT8_SNGL_PRES_RES_CHECKSUM:
        case BIT8_CRC:
            //printf("First option.\n");
            wordSize = 12;
            get_raw_mask_16bit(wordSize, corruptionType, corruptionTypeOption, &returnVal);
            //printf("From mask calculation:\n");
            //printf("Mask: %d.\n", returnVal);
            adjust_for_12_bits(&returnVal);
            //printf("Adjusted mask: %d.\n", returnVal);
            break;
        case BIT8_DBL_PRES_CHECKSUM:
        case BIT8_HONEYWELL_CHECKSUM:
            //printf("Second option.\n");
            wordSize = 16;
            get_raw_mask_16bit(wordSize, corruptionType, corruptionTypeOption, &returnVal);
            break;
        default:
    }
    return returnVal;
}

void apply_16_bit_mask(const uint16_t mask, uint8_t dwCW[])
{
    uint8_t topMask = (uint8_t)((mask>>8)&0xFF);
    uint8_t bottomMask = (uint8_t)(mask&0xFF);
    
    //printf("Intput mask: %d.\n", mask);
    //printf("Input dwCW[0]: %d.\n", dwCW[0]);
    //printf("input dwCW[1]: %d.\n", dwCW[1]);

    // Apply the mask.
    dwCW[0] = dwCW[0] ^ topMask;
    dwCW[1] = dwCW[1] ^ bottomMask;
    //printf("Adjusted dwCW[0]: %d.\n", dwCW[0]);
    //printf("Adjusted dwCW[1]: %d.\n", dwCW[1]);
}

void get_buffer_after_space(const char in_buffer[], char out_buffer[], const int size)
{
    uint8_t start_copy = 0;
    const char acceptable_values[NUM_ACCPET_VALUES] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    int out_buff_pointer = 0;
    // Initialize the out_buffer.
    for (int i = 0; i < size; i++)
    {
        out_buffer[i] = ' ';
    }

    for(int i = 0; i < size; i++)
    {
        if (start_copy == 1)
        {
            for (int j = 0; j < NUM_ACCPET_VALUES; j++)
            {
                if(in_buffer[i] == acceptable_values[j])
                {
                    out_buffer[out_buff_pointer] = in_buffer[i];
                    out_buff_pointer++;
                    break;
                }
            }
        }
        if (in_buffer[i] ==  ' ')
        {
            start_copy = 1;
        }
    }
}

#endif