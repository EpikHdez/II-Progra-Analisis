#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR_SIZE 2048

int readFile(char *destiny, char fileName[])
{
    char buffer[MAX_CHAR_SIZE];
    FILE *file = fopen(fileName, "r");

    if(!file)
        return 1;

    fgets(buffer, sizeof(buffer), file);
    strcpy(destiny, buffer);

    return 0;
}

int max(int x, int y, int z)
{
    int m1 = x > y ? x : y;
    int m2 = y > z ? y : z;

    return m1 > m2 ? m1 : m2;
}

int needleman_Wunsch(char h1[], char h2[])
{
    int i, j, match, insert, erase;
    int length1 = strlen(h1);
    int length2 = strlen(h2);
    int F[length1][length2];

    for(i = 0; i < length1; i++)
       F[i][0] = -i;

    for(j = 0; j < length2; j++)
        F[0][j] = -j;

    for(i = 1; i < length1; i++)
    {
        for(j = 1; j < length2; j++)
        {
            match = F[i-1][j-1] + (h1[i-1] == h2[j-1] ? 1 : -1);
            insert = F[i-1][j] - 1;
            erase = F[i][j-1] - 1;
            F[i][j] = max(match, insert, erase);
        }
    }

    for(i = 0; i < length1; i++)
    {
        for(j = 0; j < length2; j++)
            printf("%d | ", F[i][j]);

        printf("\n");
    }

    return F[length1-1][length2-1];
}

int main(int argc, char *argv[])
{
    char h1[MAX_CHAR_SIZE], h2[MAX_CHAR_SIZE];
    int answer = 0;

    readFile(h1, argv[1]);
    readFile(h2, argv[2]);

    printf("Calculando...\n");
    answer = needleman_Wunsch(h1, h2);
    printf("El resultado es: %d\n\n", answer);

    return 0;
}
