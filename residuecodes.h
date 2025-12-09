#ifndef RESIDUECODES_H
#define RESIDUECODES_H
#include <stdint.h>

uint8_t lcresidarith_resalgo(uint16_t input, uint8_t a_modexp)
{
    uint8_t mask = (1<<a_modexp) - 1;
    uint8_t sum = 0;

    for(int i = 0; i < 16; i+=a_modexp)
    {
        sum += (input>>i) & mask;
    }

    return sum % mask;
}

uint16_t lowcost_residuearith_8bDW(uint8_t input, uint8_t a_modexp)
{
    uint16_t output = input;

    output <<= a_modexp;
    output |= lcresidarith_resalgo(input, a_modexp);
    return output;
}

bool lcresidarith_extract_compare(uint16_t input1, uint16_t input2, uint16_t output, uint8_t a_modexp)
{
    uint8_t mask = (1<<a_modexp) - 1;
    uint8_t input1res = input1 & mask;
    uint8_t input2res = input2 & mask;
    uint8_t outputres = lcresidarith_resalgo(output, a_modexp);

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

#endif