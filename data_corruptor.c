#include <stdio.h>
#include <stdint.h>

int main()
{

    printf("Start data corruptor.\n");
    FILE *fptrHR;
    FILE *fptrCS;
	
	fptrHR = fopen("data.txt", "a+");
    fptrCS = fopen("data.bin", "rb");

    char line[100];
    
    unsigned charToInt;
    while (fgets(line, sizeof(line), fptrHR) != NULL)
    {
        printf("%s", line);
    }

    fclose(fptrHR);

    uint8_t dwCW[2];
    while(fread(dwCW, sizeof(uint8_t), 2, fptrCS) == 2)
    {
        printf("%hhu %hhu\n", dwCW[0], dwCW[1]);
    }

    fclose(fptrCS);


    printf("End data corruptor.\n");
    return 0;

}