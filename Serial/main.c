#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define false 0
#define true 1

typedef short bool;

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
            match = F[i-1][j-1] + (h1[i] == h2[j] ? 1 : -1);
            insert = F[i-1][j] - 1;
            erase = F[i][j-1] - 1;
            F[i][j] = max(match, insert, erase);
        }
    }

    return F[length1 - 1][length2 - 1];
}

int main()
{
    char h1[1000], h2[1000];
    char userAnswer[1];
    bool keepIt = true;

    while(keepIt)
    {

    }

    return 0;
}
