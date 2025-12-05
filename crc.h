#ifndef CRC_H
#define CRC_H

// #include <stdio.h>

uint16_t CRC4_12bCW_8bDW (uint8_t dataword, uint8_t generator_polynomial_5bit)
{
    if(generator_polynomial_5bit == 0x0) generator_polynomial_5bit = 0x17;

    uint8_t crc_append = 0x0;
    uint8_t circuitbits = 0x0;

    for(int i = 0; i < 8; i++)
    {
        // printf("0x%x | ", circuitbits);
        //shift
        circuitbits = (circuitbits << 1) & 0x1F;
        //append
        uint8_t nextbit = (dataword >> (7-i)) & 0x1;
        circuitbits ^= nextbit;
        //look at first bit
        if(circuitbits & 0x10)
        {
            circuitbits ^= generator_polynomial_5bit;
        }
        // continue or xor with generator
        // printf("0x%x\n", circuitbits);
    }

    crc_append = circuitbits & 0x0F;

    return (((uint16_t)dataword) << 4) | crc_append;
}

#endif