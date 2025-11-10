#include <stdio.h>
#include <stdint.h>

int main()
{

    printf("Start data corruptor.\n");
    FILE *fptr;

	
	fptr = fopen("data.txt", "a+");

    char line[100];
    
    unsigned charToInt;
    while (fgets(line, sizeof(line), fptr) != NULL)
    {
        printf("%s", line);
    }

    fclose(fptr);
    printf("End data corruptor.\n");
    return 0;

}