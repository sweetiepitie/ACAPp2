#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define INTERACTOR 0
#define UPPER 1
#define ADDER 2
#define HIJOTONTO 3
#define COMM_TAG 0
#define EXIT_TAG 1

int main(int argc, char **argv)
{
    int size, rank;
    if (argc != 1){
        perror("Invalid number of arguments. Format required: ./program\n");
        exit(-1);
    }
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (size != 4)
    {   
        perror("4 processes are required\n");
        exit(-1);
    }

    if (rank == INTERACTOR)
    {
        int action, tamR, tamMsg, sumStr;
        char string[512];
        float Rarray[] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.10},
            res;
        MPI_Status status;
        printf("\n\nIntroduce a value: \n");
        scanf ("%d", &action);
        while(action && action > 0 && action <= 4)
        {   
            switch(action){
                case 1 :
                    printf("\nIntroduce a string: \n");
                    scanf("%s", string);
                    tamMsg = strlen(string) + 1;
                    MPI_Send(string, tamMsg, MPI_CHAR, UPPER, COMM_TAG, MPI_COMM_WORLD);
                    MPI_Recv(string, tamMsg, MPI_CHAR, UPPER, COMM_TAG, MPI_COMM_WORLD, &status);
                    printf("\nNew string is %s\n", string);
                
                break;
                case 2 :
                    MPI_Send(Rarray, 10, MPI_FLOAT, ADDER, COMM_TAG, MPI_COMM_WORLD);
                    MPI_Recv(&res, 1, MPI_FLOAT, ADDER, COMM_TAG, MPI_COMM_WORLD, &status);
                    printf("Square root of array sum of an array of real numbers is %f\n", res);
                
                break;
                case 3 : 
                    MPI_Send(&action, 1, MPI_INT, HIJOTONTO, COMM_TAG, MPI_COMM_WORLD);
                    MPI_Recv(&sumStr, 1, MPI_INT, HIJOTONTO, COMM_TAG, MPI_COMM_WORLD, &status);
                    printf("The sum is %d\n", sumStr);
                
                break;
                default : break;
            }

            printf("Introduce a value: \n");
            scanf ("%d", &action);
        }

        for (int i = 0; i < size; i++)
            MPI_Send(&action, 1, MPI_CHAR, i, EXIT_TAG, MPI_COMM_WORLD);
    
    }else if (rank == UPPER){

        char stringR[512], stringS[512], c[2];
        int tam;
        MPI_Status status;
        
        MPI_Probe(INTERACTOR, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_CHAR, &tam);
        while (status.MPI_TAG != EXIT_TAG)
        {   
            MPI_Recv(stringR, tam, MPI_CHAR, INTERACTOR, COMM_TAG, MPI_COMM_WORLD, &status);
            for (int i = 0; i < tam - 1; i++){
                sprintf(c, "%c", toupper(stringR[i]));
                strcat(stringS, c);
            }

            MPI_Send(stringS, tam, MPI_CHAR, INTERACTOR, COMM_TAG, MPI_COMM_WORLD);
            MPI_Probe(INTERACTOR, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_CHAR, &tam);

            strcpy(stringS, "");
        }

    }else if (rank == ADDER){

        float arrayR[10], res;
        MPI_Status status;
        

        MPI_Probe(INTERACTOR, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        while (status.MPI_TAG != EXIT_TAG)
        {   
            res = 0;
            MPI_Recv(arrayR, 10, MPI_FLOAT, INTERACTOR, COMM_TAG, MPI_COMM_WORLD, &status);
            for (int i = 0; i < 10; i++)
                res += arrayR[i];
            
            res = sqrtf(res);
            MPI_Send(&res, 1, MPI_FLOAT, INTERACTOR, COMM_TAG, MPI_COMM_WORLD);
            MPI_Probe(INTERACTOR, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }  
    
    }else if (rank == HIJOTONTO){

        char msg[] = "Entrando en funcionalidad 3";
        int tam = strlen(msg) - 1, action, sum;
        MPI_Status status;
        
        MPI_Probe(INTERACTOR, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        while (status.MPI_TAG != EXIT_TAG)
        {   
            sum = 0;
            MPI_Recv(&action, 1, MPI_INT, INTERACTOR, COMM_TAG, MPI_COMM_WORLD, &status);
            printf("Process n 3 received a request, gonna summ the following string %s\n", msg);
            for (int i = 0; i < tam - 1; i++)
                sum += msg[i];

            MPI_Send(&sum, 1, MPI_INT, INTERACTOR, COMM_TAG, MPI_COMM_WORLD);
            MPI_Probe(INTERACTOR, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }
    
    }

    MPI_Finalize();

    return 0;
}