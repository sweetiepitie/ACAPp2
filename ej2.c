#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>
#include "pgm.h"

#define MASTER 0
#define TAG 0

double get_wall_time(){
	struct timeval t;
	if (gettimeofday(&t, NULL)){
		printf("lol\n");
		return 0;
	}

	return (double)t.tv_sec + (double)t.tv_usec * .000001;
}

void convolucion(unsigned char** Original, int** nucleo, unsigned char** Salida, int Largo, int Alto) {
    int x, y;
    int suma;
    int k = 0;
    int i, j;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            k += nucleo[i][j];

    for (x = 1; x < Largo-1; x++){
        for (y = 1; y < Alto-1; y++){
            printf("\nAlto : %d\n", y);
            suma = 0;
            for (i = 0; i < 3; i++){
                for (j = 0; j < 3; j++){
                    suma = suma + Original[(x-1)+i][(y-1)+j] * nucleo[i][j];
                }
            }
            if(k==0)
                Salida[x][y] = suma;
            else
                Salida[x][y] = suma/k;
        }
    }
}

int main(int argc, char ** argv)
{
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int** nucleo = (int**) GetMem2D(3, 3, sizeof(int));
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            nucleo[i][j] = -1;
    nucleo[1][1] = 1;

    if (!rank)
    {
        // Reading the image, sending data to each process
        MPI_Status status;
        int Largo, Alto, cols_per_process, rest, rest_sum;
        double wall0, wall1;
        printf("\n\n");
        wall0 = get_wall_time();
        unsigned char** Original = pgmread("imagenes/lena_original.pgm", &Largo, &Alto);
        unsigned char** Salida   = (unsigned char**)GetMem2D(Largo, Alto, sizeof(unsigned char));
        cols_per_process = (Largo) / size;
        rest = (Largo) % size;

        MPI_Bcast(&Largo, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
        for (int i = 1; i < size; i++){
            rest_sum = Alto*(cols_per_process + (i <= rest));
            MPI_Send(Original[i*(cols_per_process)+(i<=rest)], rest_sum, MPI_UNSIGNED_CHAR, i, TAG, MPI_COMM_WORLD);
        }
        convolucion(Original, nucleo, Salida, Largo, Alto);
        for (int i = 1; i < size; i++){
            rest_sum = Alto*(cols_per_process + (i <= rest));
            MPI_Recv(Salida[i*(cols_per_process)+(i<=rest)], rest_sum, MPI_UNSIGNED_CHAR, i, TAG, MPI_COMM_WORLD, &status);
        }

        pgmwrite(Salida, "imagenes/lena_procesada1.pgm", Largo, Alto);

        wall1 = get_wall_time();
        printf("Wall time consumed : %f\n", wall1 - wall0);

        Free2D((void**) nucleo, 3);
        Free2D((void**) Original, Largo);
        Free2D((void**) Salida, Largo);

    }else{
        int Largo, Alto, count; 
        MPI_Status status;
        MPI_Bcast(&Alto, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
        MPI_Probe(MASTER, TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &count);
        Largo = count / Alto;
        printf("Largo recibido %d, Alto calculado %d\n", Largo, Alto);
        unsigned char **Original = (unsigned char **)GetMem2D(Largo, Alto,sizeof(unsigned char));
        unsigned char **Salida = (unsigned char **)GetMem2D(Largo, Alto, sizeof(unsigned char));

        MPI_Recv(Original[0], count, MPI_UNSIGNED_CHAR, MASTER, TAG, MPI_COMM_WORLD, &status);
        convolucion(Original, nucleo, Salida, Largo, Alto);
        MPI_Send(Salida[0], Largo*Alto, MPI_UNSIGNED_CHAR, MASTER, TAG, MPI_COMM_WORLD);
        
        Free2D((void**) nucleo, 3);
        Free2D((void**) Original, Largo);
        Free2D((void**) Salida, Largo);
    }

    MPI_Finalize();
    return 0;
}