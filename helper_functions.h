#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "types.h"
#define NUM_ACCPET_VALUES 10

void convert_uint16_t_to_bit_string(uint16_t input, char helper[])
{
    for (int i = 15; i >= 0; i--)
    {
        if(((0x1<<i)&input) != 0)
        {
            helper[15-i] = '1';
        }
    }
}

void adjust_for_12_bits(uint16_t *input)
{
    uint16_t top = (*input & 0xFF00);
    uint16_t bottom = (*input & 0x00FF);
    uint16_t intermediate;
    top <<= 4;
    top >>= 4;
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
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    srand(ts.tv_nsec);
    switch(corruptionType)
    {
        int position;
        case CORR_SNGL_BIT:
            position = rand() % wordSize;
            *returnVal = 0x1 << position;
            break;
        case CORR_BURST:
            int possiblePositions = wordSize-corruptionTypeOption+1;
            position = rand() % possiblePositions;
            *returnVal = build_burst_error_16bit(corruptionTypeOption);
            *returnVal = *returnVal << position;
            break;
        case CORR_RAND:
            // Initialize vector;
            int8_t positions[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
            for (int i = 0; i < 16; i++)
            {
                positions[i] = -1;
            }

            uint8_t counter = 0;
            while(1)
            {
                int position = rand() % wordSize;
                // 16 possible positions.
                for (int i = 0; i < 16; i++)
                {
                    // The position is already accounted for.
                    if (positions[i] == position)
                    {
                        break;
                    }
                    else if (positions[i] == -1)
                    {
                        positions[i] = position;
                        counter++;
                        break;
                    }
                }

                // Break if we have reached our number of flips.
                if (counter >= corruptionTypeOption)
                {
                    break;
                }
            }

            // Build the mask.
            *returnVal = 0;
            for (int i = 0; i < counter; i++)
            {
                *returnVal = *returnVal | (0x1 << positions[i]);
            }
            break;
        case CORR_NONE:
            *returnVal = 0;
            break;
    }
}

uint16_t calculate_16bit_mask(int configNumber, int corruptionType, int corruptionTypeOption)
{
    uint16_t returnVal = 0;
    int wordSize;
    switch(configNumber)
    {
        // These all follow the format of 8 bit data then 4 bit code. However, both data and code use 8 bit numbers.
        case BIT8_SNGL_PRES_CHECKSUM:
        case BIT8_SNGL_PRES_RES_CHECKSUM:
            wordSize = 12;
            get_raw_mask_16bit(wordSize, corruptionType, corruptionTypeOption, &returnVal);
            adjust_for_12_bits(&returnVal);
            break;

        case BIT8_CRC:
        case BIT8_HC_SEC:
            wordSize = 12;
            get_raw_mask_16bit(wordSize, corruptionType, corruptionTypeOption, &returnVal);
            break;

        case BIT8_HC_SECDED:
            wordSize = 13;
            get_raw_mask_16bit(wordSize, corruptionType, corruptionTypeOption, &returnVal);
            break;

        case BIT8_DBL_PRES_CHECKSUM:
            wordSize = 16;
            get_raw_mask_16bit(wordSize, corruptionType, corruptionTypeOption, &returnVal);
            break;

        case BIT8_HONEYWELL_CHECKSUM:
        case BIT8_RESID_ARITH:
            wordSize = 8;
            get_raw_mask_16bit(wordSize, corruptionType, corruptionTypeOption, &returnVal);
            break;

            
        default:
            break;
    }
    //printf("Mask inside: %d.\n", returnVal);
    return returnVal;
}

void apply_16_bit_mask(const uint16_t mask, uint8_t dwCW[])
{
    uint8_t topMask = (uint8_t)((mask>>8)&0xFF);
    uint8_t bottomMask = (uint8_t)(mask&0xFF);

    // Apply the mask.
    dwCW[0] = dwCW[0] ^ topMask;
    dwCW[1] = dwCW[1] ^ bottomMask;
}

void human_readable_tokenizer_8b(uint8_t nums[], char line[])
{
    const char delim[] = " ";
    char* token;

    token = strtok(line, delim);
    nums[0] = atoi(token);
    token = strtok(NULL, delim);
    nums[1] = atoi(token);
    token = strtok(NULL, delim);
    nums[2] = atoi(token);
}

void human_readable_tokenizer_16b(uint16_t nums[], char line[])
{
    const char delim[] = " ";
    char* token;

    token = strtok(line, delim);
    nums[0] = atoi(token);
    token = strtok(NULL, delim);
    nums[1] = atoi(token);
    token = strtok(NULL, delim);
    nums[2] = atoi(token);
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

float timediff_us(struct timeval start, struct timeval end)
{
    return (float)(end.tv_sec - start.tv_sec) * 1000000.0 + (float)(end.tv_usec - start.tv_usec);
}

uint8_t get_parity(uint32_t data, uint32_t size)
{
    uint8_t sumOnes = 0;
    // printf("data: %x -- ", data);
    for(int i = 0; i < size; i++)
    {
        sumOnes += (data>>i & 0x1);
    }

    // printf("numOnes: %d\n", sumOnes);

    return sumOnes & 0x1;
}
#endif