#include <stdio.h>

int main()
{

    FILE *fptr;

	
	fptr = fopen("data.txt", "w");

    for (int i = 0; i < 256; i++)
    {
        fprintf(fptr, "%d\n", i);
    }

    fclose(fptr);
    
    return 0;

}