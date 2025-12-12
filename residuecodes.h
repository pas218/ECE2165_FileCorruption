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

    uint8_t Amask = (1<<a)-1;
    uint8_t Bmask = (1<<b)-1;

    uint8_t sum = 0;
    uint16_t output = input;

    for(int i = 0; i < 8; i+=a)
    {
        sum += (input>>i) & Amask;
    }

    uint8_t append = sum % Amask;
    output <<= a;
    output |= append;

    sum = 0;
    for(int i = 0; i < 8; i+=b)
    {
        sum += (input>>i) & Bmask;
    }

    append = sum % Bmask;
    output <<= b;
    output |= append;

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

#endif