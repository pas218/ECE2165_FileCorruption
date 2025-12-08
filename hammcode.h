#ifndef HAMMCODE_H
#define HAMMCODE_H

#include <stdio.h>
#include "helper_functions.h"

uint16_t HC_12bCW_8bDW (uint8_t dataword)
{
    uint8_t P[4] = {0b10111010, 0b11100110, 0b01110001, 0b11011101};
    uint16_t codeword = dataword << 4;

    for(int i = 0; i < 4; i++)
    {
        uint8_t xorMalMult = P[i] & dataword;
        uint8_t rowPar = get_parity(xorMalMult, 8);
        codeword |= (rowPar<<(3-i));
    }

    return codeword;
}

bool HC_12bCW_8bDW_syndrome_SEC (uint16_t* codeword)
{
    uint16_t H[4] = {0b101110101000, 
                     0b111001100100, 
                     0b011100010010, 
                     0b110111010001};
    uint8_t syndromeLookup[16] = { 255, 0, 1, 4, 2, 6, 255, 10, 3, 7, 255, 8, 5, 11, 9, 255};
    uint8_t syndrome = 0;

    for(int i = 0; i < 4; i++)
    {
        uint16_t xorMalMult = H[i] & *codeword;
        uint8_t rowPar = get_parity(xorMalMult, 16);
        syndrome |= (rowPar<<(3-i));
    }

    if(syndrome)
    {
        uint8_t wrongBit = syndromeLookup[syndrome];
        if(wrongBit == 255)
        {
            // printf("ERROR WITH HC - %d, syndrome was %d\n", *codeword, syndrome);
        }
        else
        {
            (*codeword) ^= 0x1<<wrongBit;
            // printf("fixed codeword: %d\n", *codeword);
        }
    }


    return syndrome;
}

#endif