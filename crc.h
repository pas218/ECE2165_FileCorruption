#ifndef CRC_H
#define CRC_H
#include <stdint.h>
#define CRC4_BITS 4
#define CRC4_ITERATIONS 8

// P is hard coded to 11101
uint8_t crc4_8bit(uint8_t input)
{
    int size_P = 5;
    uint8_t P = 29; // 11101
    uint8_t crc = 0;
    uint8_t helper_
    // Inititial state.
    for (int i = 0; i < CRC4_BITS; i++)
    {
        if ((crc ))
    }
    for (int i = 0; i < CRC4_ITERATIONS; i++)
    {

    }
    
    return crc & 0xF; 
}

#endif