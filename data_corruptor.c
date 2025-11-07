#include <stdio.h>

int main()
{

    FILE *fptr;

	
	fptr = fopen("data.txt", "a+");

    char line[100];
    
    while (fgets(line, sizeof(line), fptr) != NULL)
    {
        printf("%s", line);
    }

    fclose(fptr);
    
    return 0;

}