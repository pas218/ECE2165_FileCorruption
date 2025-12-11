#ifndef HAMMCODE_H
#define HAMMCODE_H

#include <stdio.h>
#include "helper_functions.h"

uint16_t HC_12bCW_8bDW (uint8_t dataword)
{
    uint8_t P[4] = {0b10111010, 0b11100110, 0b01110001, 0b11011101};
    uint16_t codeword = dataword << 4;
    // printf("%hu --> ", codeword);

    for(int i = 0; i < 4; i++)
    {
        uint8_t xorMalMult = P[i] & dataword;
        uint8_t rowPar = get_parity(xorMalMult, 8);
        codeword |= (rowPar<<(3-i));
    }

    // printf("%hu\n", codeword);
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
        uint16_t xorMatMult = H[i] & *codeword;
        uint8_t rowPar = get_parity(xorMatMult, 12);
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

uint16_t HC_13bCW_8bDW (uint8_t dataword)
{
    uint8_t P[4] = {0b10111010, 0b11100110, 0b01110001, 0b11011101};
    uint16_t codeword = dataword << 4;

    for(int i = 0; i < 4; i++)
    {
        uint8_t xorMalMult = P[i] & dataword;
        uint8_t rowPar = get_parity(xorMalMult, 8);
        codeword |= (rowPar<<(3-i));
    }

    uint8_t totalPar = get_parity(codeword, 12);
    codeword <<= 1;
    codeword |= totalPar;

    return codeword;
}

uint8_t HC_13bCW_8bDW_syndrome_SECDED (uint16_t* codeword, uint8_t* crcSynd)
{
    uint16_t H[4] = {0b101110101000, 
                     0b111001100100, 
                     0b011100010010, 
                     0b110111010001};
    uint8_t syndromeLookup[16] = { 0, 1, 2, 5, 3, 7, 255, 11, 4, 8, 255, 9, 6, 12, 10, 255};
    uint8_t syndrome = 0;
    uint8_t secorded = get_parity(*codeword, 13);
    // printf("err codeword: %d, parity: %d\n", *codeword, secorded);
    uint16_t codewordNoTotalPar = *codeword >> 1;

    for(int i = 0; i < 4; i++)
    {
        uint16_t xorMalMult = H[i] & codewordNoTotalPar;
        uint8_t rowPar = get_parity(xorMalMult, 16);
        syndrome |= (rowPar<<(3-i));
    }

    if(secorded)
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
    else
    {
        // printf("Double error detected, syndrome: %d, secded status: %d\n", syndrome, secorded);
    }

    *crcSynd = syndrome;


    return secorded;
}

uint64_t HC_38bCW_32bDW (uint32_t dataword)
{
    uint32_t P[6] = {
        0b10011100111001010011001001010101, 
        0b00111011011010010100100111000011, 
        0b11001010110010100010101011101101, 
        0b01001101100101010100111010011100,
        0b10101110001100101001011011011011,
        0b01110111100100111010101110100001
    };
    uint64_t codeword = (uint64_t)dataword << 6;
    // printf("%hu --> ", codeword);

    for(int i = 0; i < 6; i++)
    {
        uint32_t xorMalMult = P[i] & dataword;
        uint8_t rowPar = get_parity(xorMalMult, 32);
        codeword |= (rowPar<<(5-i));
    }

    // printf("%lx\n", codeword);
    return codeword;
}

bool HC_38bCW_32bDW_syndrome_SEC (uint64_t* codeword)
{
    uint64_t H[6] = {
        0b10011100111001010011001001010101100000, 
        0b00111011011010010100100111000011010000, 
        0b11001010110010100010101011101101001000, 
        0b01001101100101010100111010011100000100,
        0b10101110001100101001011011011011000010,
        0b01110111100100111010101110100001000001
    };

    uint8_t syndromeLookup[64] = 
    {
        255,   0,   1,  21,   2, 255,  16,  26,   3,  11, 255,  23, 255,  36,   9, 255,
          4,  14,   7,  35,  20,  30, 255, 255,  25, 255, 255,  31, 255,  17, 255,  13,
          5, 255,  18, 255,  24, 255,  10,  32, 255,  19,  37, 255,   8,  29, 255,  15,
        255,  34,  27, 255, 255,  22, 255, 255,  28, 255,  12,   6, 255, 255,  33, 255
    };

    uint8_t syndrome = 0;

    for(int i = 0; i < 6; i++)
    {
        uint64_t xorMatMult = H[i] & *codeword;
        uint8_t rowPar = get_parity(xorMatMult, 38);
        syndrome |= (rowPar<<(5-i));
    }

    if(syndrome)
    {
        uint8_t wrongBit = syndromeLookup[syndrome];
        if(wrongBit == 255)
        {
            // printf("ERROR WITH HC - %lx, syndrome was %d\n", *codeword, syndrome);
        }
        else
        {
            (*codeword) ^= 0x1<<wrongBit;
            // printf("fixed codeword: %d\n", *codeword);
        }
    }

    return syndrome;
}

uint16_t HC_39bCW_32bDW (uint32_t dataword)
{
    uint32_t P[6] = {
        0b10011100111001010011001001010101, 
        0b00111011011010010100100111000011, 
        0b11001010110010100010101011101101, 
        0b01001101100101010100111010011100,
        0b10101110001100101001011011011011,
        0b01110111100100111010101110100001
    };
    uint64_t codeword = (uint64_t)dataword << 4;
    // printf("%hu --> ", codeword);

    for(int i = 0; i < 6; i++)
    {
        uint32_t xorMalMult = P[i] & dataword;
        uint8_t rowPar = get_parity(xorMalMult, 32);
        codeword |= (rowPar<<(5-i));
    }

    uint8_t totalPar = get_parity(codeword, 36);
    codeword <<= 1;
    codeword |= totalPar;

    // printf("%hu\n", codeword);
    return codeword;
}

// uint8_t syndromeLookup[64] = 
// { //chunks of 16 (0-15, 16-31, 32-47, 48-63)
//     255, 255, 255,  15, 255, 255,  10,  20, 255,   5, 255,  17, 255,  30,   3, 255,
//     255,   8,   1,  29,  14,  24, 255, 255,  19, 255, 255,  25, 255,  11, 255,   7,
//     255, 255,  12, 255,  18, 255,   4,  26, 255,  13,  31, 255,   2,  23, 255,   9,
//     255,  28,  21, 255, 255,  16, 255, 255,  22, 255,   6,   0, 255, 255,  27, 255
// };

#endif