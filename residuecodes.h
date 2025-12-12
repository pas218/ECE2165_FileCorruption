#ifndef RESIDUECODES_H
#define RESIDUECODES_H
#include <stdint.h>

uint8_t lcresidarith_resalgo_16b(uint16_t input, uint8_t a_modexp)
{
    uint8_t mask = (1<<a_modexp) - 1;
    uint8_t sum = 0;

    for(int i = 0; i < 16; i+=a_modexp)
    {
        sum += (input>>i) & mask;
    }

    return sum % mask;
}

uint8_t lcresidarith_resalgo_64b(uint64_t input, uint8_t a_modexp)
{
    uint16_t mask = (1<<a_modexp) - 1;
    uint16_t sum = 0;

    for(int i = 0; i < 64; i+=a_modexp)
    {
        sum += (input>>i) & mask;
    }

    return sum % mask;
}

uint16_t lowcost_residuearith_8bDW(uint8_t input, uint8_t a_modexp)
{
    uint16_t output = input;

    output <<= a_modexp;
    output |= lcresidarith_resalgo_16b(input, a_modexp);
    return output;
}

bool lcresidarith_extract_compare_8b(uint16_t input1, uint16_t input2, uint16_t output, uint8_t a_modexp)
{
    uint8_t mask = (1<<a_modexp) - 1;
    uint8_t input1res = input1 & mask;
    uint8_t input2res = input2 & mask;
    uint8_t outputres = lcresidarith_resalgo_16b(output, a_modexp);

    bool matchRes = ((input1res + input2res) % mask) == outputres;

    // printf("output: %d, res1: %d, res2: %d, outRes: %d, matchRes: %d\n", output, input1res, input2res, outputres, matchRes);

    return matchRes;

}

uint16_t biresidue_correction_8bDW_12bCW(uint8_t input)
{
    uint8_t a = 3;
    uint8_t b = 4;

    uint16_t output = input;

    output <<= a;
    output |= lcresidarith_resalgo_16b(input, a);

    output <<= b;
    output |= lcresidarith_resalgo_16b(input, b);

    return output;
}

bool biresidue_compare_correct_8b(uint16_t input1, uint16_t input2, uint16_t output)
{
    int16_t biresidue_lut[15][7] = 
    {
        {0U},
        {0, 1, 16, 0, 256, 0, 0},
        {0, 512, 2, 0, 32, 0, 0},
        {0U},
        {0, 64, 1024, 0, 4, 0, 0},
        {0U},
        {0U},
        {0, 0, 0, -2048, 0, -128, -8},
        {0, 8, 128, 0, 2048, 0, 0},
        {0U},
        {0U},
        {0, 0, 0, -4, 0, -1024, -64},
        {0U},
        {0, 0, 0, -32, 0, -2, -512},
        {0, 0, 0, -256, 0, -16, -1},
    };

    uint8_t a = 3;
    uint8_t b = 4;

    uint8_t Amask = (1<<a)-1;
    uint8_t Bmask = (1<<b)-1;

    uint8_t input1resb = input1 & Bmask;
    uint8_t input2resb = input2 & Bmask;

    input1 >>= b;
    input2 >>= b;

    uint8_t input1resa = input1 & Amask;
    uint8_t input2resa = input2 & Amask;

    uint8_t outputresa = lcresidarith_resalgo_16b(output, 3);
    uint8_t outputresb = lcresidarith_resalgo_16b(output, 4);

    uint8_t syndromeA = outputresa - ((input1resa + input2resa) % Amask);
    if(syndromeA > Amask) syndromeA += Amask;

    uint8_t syndromeB = outputresb - ((input1resb + input2resb) % Bmask);
    if(syndromeB > Bmask) syndromeB += Bmask;

    //lookup table for correction
    int16_t lutModifier = biresidue_lut[syndromeB][syndromeA];
    output -= lutModifier;

    // printf("output: %d, lutMod: %d, syndA: %d, syndB: %d\n", output, lutModifier, syndromeA, syndromeB);

    return lutModifier != 0;

}

uint64_t lowcost_residuearith_32bDW(uint32_t input, uint8_t a_modexp)
{
    uint64_t output = input;

    output <<= a_modexp;
    output |= lcresidarith_resalgo_64b(input, a_modexp);
    return output;
}

bool lcresidarith_extract_compare_32b(uint64_t input1, uint64_t input2, uint64_t output, uint8_t a_modexp)
{
    uint16_t mask = (1<<a_modexp) - 1;
    uint16_t input1res = input1 & mask;
    uint16_t input2res = input2 & mask;
    uint16_t outputres = lcresidarith_resalgo_64b(output, a_modexp);

    bool matchRes = ((input1res + input2res) % mask) == outputres;

    // printf("output: %lu, res1: %lu, res2: %lu, outRes: %lu, matchRes: %d\n", output, input1res, input2res, outputres, matchRes);

    return matchRes;

}

uint64_t biresidue_correction_32bDW_45bCW(uint32_t input)
{
    uint8_t a = 6;
    uint8_t b = 7;

    uint64_t output = input;

    output <<= a;
    output |= lcresidarith_resalgo_64b(input, a);

    output <<= b;
    output |= lcresidarith_resalgo_64b(input, b);

    return output;
}

bool biresidue_compare_correct_32b(uint64_t input1, uint64_t input2, uint64_t output)
{
    uint8_t biresidue_lut[12][7] = 
    {
        {0, 36, 30, 24, 18, 12, 6},
        {7, 1, 37, 31, 25, 19, 13},
        {14, 8, 2, 38, 32, 26, 20},
        {21, 15, 9, 3, 39, 33, 27},
        {28, 22, 16, 10, 4, 40, 34},
        {35, 29, 23, 17, 11, 5, 41},
        {41, 5, 11, 17, 23, 29, 35},
        {34, 40, 4, 10, 16, 22, 28},
        {27, 33, 39, 3, 9, 15, 21},
        {20, 26, 32, 38, 2, 8, 14},
        {13, 19, 25, 31, 37, 1, 7},
        {6, 12, 18, 24, 30, 36, 0},
    };

    uint8_t a = 6;
    uint8_t b = 7;

    uint16_t Amask = (1<<a)-1;
    uint16_t Bmask = (1<<b)-1;

    uint16_t input1resb = input1 & Bmask;
    uint16_t input2resb = input2 & Bmask;

    input1 >>= b;
    input2 >>= b;

    uint16_t input1resa = input1 & Amask;
    uint16_t input2resa = input2 & Amask;

    uint16_t outputresa = lcresidarith_resalgo_64b(output, a);
    uint16_t outputresb = lcresidarith_resalgo_64b(output, b);

    uint16_t syndromeA = outputresa - ((input1resa + input2resa) % Amask);
    if(syndromeA > Amask) syndromeA += Amask;

    uint16_t syndromeB = outputresb - ((input1resb + input2resb) % Bmask);
    if(syndromeB > Bmask) syndromeB += Bmask;

    uint8_t minLUTA;
    uint8_t minLUTB;

    //lookup table for correction
    switch(syndromeA)
    {
        case 1:
            minLUTA = 0;
            break;
        case 2:
            minLUTA = 1;
            break;
        case 4:
            minLUTA = 2;
            break;
        case 8:
            minLUTA = 3;
            break;
        case 16:
            minLUTA = 4;
            break;
        case 32:
            minLUTA = 5;
            break;
        case 31:
            minLUTA = 6;
            break;
        case 47:
            minLUTA = 7;
            break;
        case 55:
            minLUTA = 8;
            break;
        case 59:
            minLUTA = 9;
            break;
        case 61:
            minLUTA = 10;
            break;
        case 62:
            minLUTA = 11;
            break;
        default:
            minLUTA = 255;
            break;
    }

    switch(syndromeB)
    {
        case 1:
        case 63:
            minLUTB = 0;
            break;
        case 2:
        case 95:
            minLUTB = 1;
            break;
        case 4:
        case 111:
            minLUTB = 2;
            break;
        case 8:
        case 119:
            minLUTB = 3;
            break;
        case 16:
        case 123:
            minLUTB = 4;
            break;
        case 32:
        case 125:
            minLUTB = 5;
            break;
        case 64:
        case 126:
            minLUTB = 6;
            break;
        default:
            minLUTB = 255;
            break;
    }

    uint8_t lutShift = biresidue_lut[minLUTA][minLUTB];
    int64_t lutModifier;
    if(minLUTA > 5)
    {
        lutModifier = - (1<<lutShift);
    }
    else lutModifier = (1<<lutShift);
    
    output -= lutModifier;

    // printf("output: %d, lutMod: %d, syndA: %d, syndB: %d\n", output, lutModifier, syndromeA, syndromeB);

    return lutModifier != 0;

}

#endif