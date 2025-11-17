#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define BITS_PER_BYTE  8
#define BYTES_PER_CHAR 1
#define NUM_CHAR       1  
#define SIZE_DW_BITS (BITS_PER_BYTE*BYTES_PER_CHAR*NUM_CHAR)
#define MAX_DW_VALUE (1 << SIZE_DW_BITS)-1



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

int main()
{


    FILE *fptr;

	
	fptr = fopen("data.txt", "w");


    uint16_t counter = 0;
    uint8_t keepGoing = 1;
    uint8_t checksum;
    while (keepGoing == 1)
    {

        if (counter >= MAX_DW_VALUE)
        {
            keepGoing = 0;
            break;
        }
        checksum = checksum_4bitCW_8bDW_snglPrec(counter, counter+1);

        fprintf(fptr, "%hhu %hhu\n", counter, checksum);
        // fprintf(fptr, "%hhu %hhu %hhu\n", counter, counter+1, checksum);
        
        counter++;
    }

    fclose(fptr);
    
    return 0;

}