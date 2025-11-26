#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int configNumber;
    int corruptionType;
    int corruptionOption = -1; 
    
    // Corruption option is specified.
    if (argc == 4)
    {
        configNumber = atoi(argv[1]);
        corruptionType = atoi(argv[2]);
        corruptionOption = atoi(argv[3]);
    }
    // Corruption option not specified.
    else if (argc == 3)
    {
        configNumber = atoi(argv[1]);
        corruptionType = atoi(argv[2]);
    }
    else
    {
        configNumber = 1;
        corruptionType = 1;
    }

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