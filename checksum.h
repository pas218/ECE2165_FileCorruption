#ifndef CHECKSUM_H
#define CHECKSUM_H
#include <stdint.h>

uint8_t checksum_4bitCW_8bDW_snglPrec (uint8_t dataword, uint8_t sizeChecksum)
{
    uint8_t returnVal = 0;

    uint8_t bottomHalf = dataword & (0b00001111);
    uint8_t topHalf = (dataword & (0b11110000)) >> sizeChecksum;   // Make sure both halves align with each other;
    uint8_t answerBit = 0;
    uint8_t carry = 0;

    // Return the addition of bottom and top half, but only keep the first 4 bits.
    return (bottomHalf + topHalf) & 0b00001111;
}

uint8_t checksum_4bitCW_8bDW_doubPrec (uint8_t dataword, uint8_t sizeChecksum)
{
    uint8_t bottomHalf = dataword & (0b00001111);
    uint8_t topHalf = (dataword & (0b11110000)) >> sizeChecksum;   // Make sure both halves align with each other;

    // Return the addition of bottom and top half, but keep 8 bits.
    return (bottomHalf + topHalf) & 0b11111111;
}

uint8_t residueChecksum_4bitCW_8bDW_snglPrec (uint8_t dataword, uint8_t sizeChecksum)
{
    uint8_t bottomHalf = dataword & (0b00001111);
    uint8_t topHalf = (dataword & (0b11110000)) >> sizeChecksum;   // Make sure both halves align with each other;

    uint8_t carryIso = (bottomHalf + topHalf) & 0b11110000;
    uint8_t carry = carryIso >> sizeChecksum;

    return (bottomHalf + topHalf + carry) & 0b00001111;  
}

uint8_t HWChecksum_4bitCW_8bDW (uint8_t dataword1_2, uint8_t dataword3_4)
{
    // innately double precision
    return dataword1_2 + dataword3_4;
}

#endif