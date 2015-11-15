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
    dest[size] = '\0';
}

//Obtener el maximo de tres valores
int max(int x, int y, int z)
{
    int m1 = x > y ? x : y;
    int m2 = y > z ? y : z;

    return m1 > m2 ? m1 : m2;
}

void needlemanWunsch(char h1[], char h2[], int rank, int size, int blockA)
{
    int i, j, k, match, insert, erase;
    int length1 = (strlen(h1) + 1);
    int length2 = (strlen(h2) + 1);
    int reference[blockA];
    int F[length1][length2];

    printf("En proceso %d: h1 = %s y h2 = %s\n", rank, h1, h2);

    if(rank == 0)
    {
        for(i = 0; i < length1; i++)
            F[i][0] = -i;

        for(j = 0; j < length2; j++)
            F[0][j] = -j;

        MPI_Send(&j, sizeof(j), MPI_INT, 1, 0, MPI_COMM_WORLD);
        k = 1;

        for(i = 1; i < length1; i++)
        {
            for(j = 1; j < length2; j++)
            {
                match = F[i-1][j-1] + (h1[i-1] == h2[j-1] ? 1 : -1);
                insert = F[i-1][j] - 1;
                erase = F[i][j-1] - 1;
                F[i][j] = max(match, insert, erase);
            }

            if(k == blockA)
            {
                k = (i-blockA);

                for(j = 0; j <= blockA; j++)
                    reference[j] = F[k++][length2-1];

                k = 0;
                MPI_Send(reference, sizeof(reference), MPI_INT, 1, 0, MPI_COMM_WORLD);
            }

            k++;
        }

        //Mostrar la matriz
        for(i = 0; i < length1; i++)
        {
            for(j = 0; j < length2; j++)
                printf("%d | ", F[i][j]);

            printf("\n");
        }
    }
    else
    {
        MPI_Recv(&j, sizeof(j), MPI_INT, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for(i = 1; i < length2; i++)
            F[0][i] = -j++;

        if(rank < (size-1))
            MPI_Send(&j, sizeof(j), MPI_INT, (rank+1), 0, MPI_COMM_WORLD);

        for(i = 1; i < length1; i+=blockA)
        {
            MPI_Recv(reference, sizeof(reference), MPI_INT, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            k = (i-1);
            for(j = 0; j <= blockA; j++)
                F[k++][0] = reference[j];

            for(j = i; j <= (blockA*i); j++)
            {
                for(k = 1; k < length2; k++)
                {
                    match = F[j-1][k-1] + (h1[j-1] == h2[k-1] ? 1 : -1);
                    insert = F[j-1][k] - 1;
                    erase = F[j][k-1] - 1;
                    F[j][k] = max(match, insert, erase);
                }
            }

            if(rank < (size-1))
            {
                k = (i-1);
                for(j = 0; j <= blockA; j++)
                    reference[j] = F[k++][length2-1];

                MPI_Send(reference, sizeof(reference), MPI_INT, (rank+1), 0, MPI_COMM_WORLD);
            }
        }

        for(i = 0; i < length1; i++)
        {
            for(j = 0; j < length2; j++)
                printf("%d | ", F[i][j]);

            printf("\n");
        }


        // MPI_Recv(&j, sizeof(j), MPI_INT, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // MPI_Recv(reference, sizeof(reference), MPI_INT, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // for(i = 0; i <= blockA; i++)
        //     F[i][0] = reference[i];

        // for(i = 1; i < length2; i++)
        //     F[0][i] = -j++;

        // if(rank < (size-1))
        // {
        //     MPI_Send(&j, sizeof(j), MPI_INT, (rank+1), 0, MPI_COMM_WORLD);
        // }

        // //Mostrar la matriz
        // for(i = 0; i < length1; i++)
        // {
        //     for(j = 0; j < length2; j++)
        //         printf("%d | ", F[i][j]);

        //     printf("\n");
        // }
    }
}


int main(int argc, char *argv[])
{
    int rank, size, blockA, blockB, i;
    char h1Complete[MAX_CHAR_SIZE], h2Complete[MAX_CHAR_SIZE];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0)
    {
        if (readFile(h1Complete, argv[1]) != 0 || readFile(h2Complete, argv[2]) != 0)
        {
            printf("No se pudieron leer los archivos de datos, intentelo de nuevo.\n");
            return 1;
        }

        blockA = (strlen(h1Complete) / size);
        blockB = (strlen(h2Complete) / size);
        char h2[blockB];

        getSubStr(h2, h2Complete, blockB);

        MPI_Send(&blockA, sizeof(blockA), MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(&blockB, sizeof(blockB), MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(h1Complete, sizeof(h1Complete), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        MPI_Send(h2Complete, sizeof(h2Complete), MPI_CHAR, 1, 0, MPI_COMM_WORLD);

        needlemanWunsch(h1Complete, h2, rank, size, blockA);
    }
    else
    {
        MPI_Recv(&blockA, sizeof(blockA), MPI_INT, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&blockB, sizeof(blockB), MPI_INT, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(h1Complete, sizeof(h1Complete), MPI_CHAR, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(h2Complete, sizeof(h2Complete), MPI_CHAR, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        char h2[blockB];
        getSubStr(h2, h2Complete, blockB);

        if(rank < (size-1))
        {
            MPI_Send(&blockA, sizeof(blockA), MPI_INT, (rank+1), 0, MPI_COMM_WORLD);
            MPI_Send(&blockB, sizeof(blockB), MPI_INT, (rank+1), 0, MPI_COMM_WORLD);
            MPI_Send(h1Complete, sizeof(h1Complete), MPI_CHAR, (rank+1), 0, MPI_COMM_WORLD);
            MPI_Send(h2Complete, sizeof(h2Complete), MPI_CHAR, (rank+1), 0, MPI_COMM_WORLD);
        }

        needlemanWunsch(h1Complete, h2, rank, size, blockA);
    }

    MPI_Finalize();
    return 0;
}
