#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_CHAR_SIZE 2048

/*Metodo para la lectura de los archivos que contienen la cadena da caracteres que se van a comparar.
Solamente se lee la primera linea, por lo que toda la hilera debe estar en la primera linea y 
con un máximo de 2048 carácteres.*/
int readFile(char *dest, char fileName[])
{
    unsigned char buffer[MAX_CHAR_SIZE];
    FILE *file = fopen(fileName, "r");

    if(!file)
        return 1;

    fgets(buffer, sizeof(buffer), file);
    strcpy(dest, buffer);
    dest[strlen(dest)-1] = '\0';

    fclose(file);
    return 0;
}

void strcut(char *dest, char *src, int startPoint)
{
    int i, j;

    i = startPoint;
    j = 0;

    for(i; i < strlen(src); i++)
        dest[j++] = src[i];

    dest[j] = '\0';
}

void getSubStr(char *dest, char *str, int size)
{
    strncpy(dest, str, size);
    strcut(str, str, size);
}

//Obtener el maximo de tres valores
int max(int x, int y, int z)
{
    int m1 = x > y ? x : y;
    int m2 = y > z ? y : z;

    return m1 > m2 ? m1 : m2;
}

void needlemanWunsch(int *F, char h1[], char h2[])
{
    int i, j, match, insert, erase;
    int length1 = strlen(h1);
    int length2 = strlen(h2);

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
}


int main(int argc, char *argv[])
{
    int rank, size, blockA, blockB;
    char h1Complete[MAX_CHAR_SIZE], h2Complete[MAX_CHAR_SIZE];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0)
    {
        readFile(h1Complete, argv[1]);
        readFile(h2Complete, argv[2]);
        printf("Las cadenas completas son: %s y %s\n", h1Complete, h2Complete);

        blockA = (strlen(h1Complete) / size);
        blockB = (strlen(h2Complete) / size);
        char h1[blockA], h2[blockB];

        getSubStr(h1, h1Complete, blockA);
        getSubStr(h2, h2Complete, blockB);
        int F[strlen(h1)][strlen(h2)];

        printf("Se leyo en %d las cadenas: %s y %s\n", rank, h1, h2);

        needlemanWunsch(&F, h1, h2);

        // char h1[blockA];
        // char h2[blockB];
        // printf("%d\n%d\n", blockA, blockB);

        // strncpy(h1, h1Complete, blockA);
        // strncpy(h2, h2Complete, blockB);
        // printf("%s\n%s\n", h1, h2);

        // strcut(h1, h1, blockA);
        // strcut(h2, h2, blockB);

        MPI_Send(&blockA, sizeof(blockA), MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(&blockB, sizeof(blockB), MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(h1Complete, sizeof(h1Complete), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        MPI_Send(h2Complete, sizeof(h2Complete), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    }

    if(rank >= 1)
    {
        MPI_Recv(&blockA, sizeof(blockA), MPI_INT, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&blockB, sizeof(blockB), MPI_INT, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(h1Complete, sizeof(h1Complete), MPI_CHAR, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(h2Complete, sizeof(h2Complete), MPI_CHAR, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Se recibio en %d los strings: %s y %s desde el proceso %d.\n", rank, h1Complete, h2Complete, (rank-1));

        char h1[blockA], h2[blockB];
        getSubStr(h1, h1Complete, blockA);
        getSubStr(h2, h2Complete, blockB);

        if(rank < (size-1))
        {
            MPI_Send(&blockA, sizeof(blockA), MPI_INT, (rank+1), 0, MPI_COMM_WORLD);
            MPI_Send(&blockA, sizeof(blockA), MPI_INT, (rank+1), 0, MPI_COMM_WORLD);
            MPI_Send(h1Complete, sizeof(h1Complete), MPI_CHAR, (rank+1), 0, MPI_COMM_WORLD);
            MPI_Send(h2Complete, sizeof(h2Complete), MPI_CHAR, (rank+1), 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
