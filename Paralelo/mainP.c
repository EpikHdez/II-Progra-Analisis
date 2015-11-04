#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int max(int x, int y, int z)
{
    int m1 = x > y ? x : y;
    int m2 = y > z ? y : z;

    return m1 > m2 ? m1 : m2;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0)
    {
        char message[12] = "Hola mundo!";
        MPI_Send(message, sizeof(message), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    }
    if(rank == 1)
    {
        char message[12];
        MPI_Recv(message, sizeof(message), MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Se recibio en %d el mensaje: %s desde el procesador %d.\n", rank, message, 0);
    }
    if(rank >= 2)
    {
        printf("Yo, el proceso %d no hago nada.\n", rank);
    }

    MPI_Finalize();
    return 0;
}
