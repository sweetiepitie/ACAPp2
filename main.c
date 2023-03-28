/*
   LÃ­nea de compilaciÃ³n: gcc main.c pgm.c -o main

   Ejemplo del uso de:

   pgmread
   pgmwrite

   Las matrices que contienen a las imÃ¡genes son declaradas como uchar **

   % % % % % % % %

   Ejemplo 1:

   unsigned char **Original = (unsigned char **)pgmread("entrada.pgm", &Largo, &Alto);
   La imagen PGM se lee de la lÃ­nea de comando (argv[1]). La funciÃ³n
   pgmread regresa tres valores:

   1. la imagen leÃ­da       (Original)
   2. el largo de la imagen (Largo)
   3. el alto de la imagen  (Alto)

   % % % % % % % %

   Ejemplo 2:

   pgmwrite(Salida, "salida.pgm", Largo, Alto);
   La imagen Salida es escrita al disco con el nombre de negativo.pgm, la
   imagen resulta en formato PGM. La imagen se escribe desde el inicio (0,0)
   hasta (Largo, Alto).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "pgm.h"

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

/* * * * *          * * * * *          * * * * *          * * * * */

int main(int argc, char *argv[]){
    int Largo, Alto;
    int i, j;
    double wall0, wall1;
    
    wall0 = get_wall_time();
    unsigned char** Original = pgmread("imagenes/lena_original.pgm", &Largo, &Alto);
    unsigned char** Salida   = (unsigned char**)GetMem2D(Largo, Alto, sizeof(unsigned char));

    int** nucleo = (int**) GetMem2D(3, 3, sizeof(int));
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            nucleo[i][j] = -1;
    nucleo[1][1] = 1;

    convolucion(Original, nucleo, Salida, Largo, Alto);

    pgmwrite(Salida, "lena_procesada1.pgm", Largo, Alto);
    wall1 = get_wall_time();

    printf("Wall time consumed : %f\n", wall1 - wall0);

    Free2D((void**) nucleo, 3);
    Free2D((void**) Original, Largo);
    Free2D((void**) Salida, Largo);
}