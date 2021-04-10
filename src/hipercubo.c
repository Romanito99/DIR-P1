/*Red hipercubo*/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "mpi.h"
#include "leerFichero.h"
#define D 4 //Dimensión de la red 
#define MAX_TAM 1000
#define MAX 1000 

void conocerVecinos(int rank, int* vecinos); 
double maximo(int rank, double bufferRank, int* vecinos);

int main(int argc, char *argv[]){
    int rank, size; 
    int numero_Nodos = pow(2,D); //indica el numero de procesos que vamos a tener dependiendo de la dimension dada 
    double bufferRank; 
    int error = 0; //0 si error la ejecucion y 1 si no. 
    int vecinos[D]; 
    MPI_Status status; 
    MPI_Request request;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    //Si soy el rank 0
    if(rank == 0){
        
        if(numero_Nodos != size){

            fprintf(stderr, "ERROR. Para un hipercubo de dimensión %d se deben lanzar %d procesos\n", D, numero_Nodos); 
            //error
            error = 1; 
            MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD); 

        }else{ 

            double *numleidos; 
            numleidos = malloc(MAX_TAM * sizeof(double)); 
            int valFichero = leerFichero(numleidos); 
    
            if(numero_Nodos != valFichero){

                fprintf(stderr, "ERROR. No hay el número correcto de valores en el fichero. Hacen falta %d valores\n", numero_Nodos); 
                //error
                error = 1; 
                MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD); 

            }else{
                 
                MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);
                //El root reparte los valores 
                for(int j = 0; j < valFichero; ++j){
                    bufferRank = numleidos[j]; 
                    MPI_Isend(&bufferRank, 1, MPI_DOUBLE, j, 0, MPI_COMM_WORLD,&request); 
                    MPI_Wait(&request, &status);
                }
            }
        }
    }
    //Esperamos al root
    MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD); 

    if(error == 0){
        
        MPI_Recv(&bufferRank, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 
        conocerVecinos(rank, vecinos); 
        double numMax = maximo(rank, bufferRank, vecinos); 
    
        if(rank == 0){
            printf("[RANK %d] El valor máximo de la red es: %.2f\n", rank, numMax); 
        }
    }
    MPI_Finalize(); 
    return EXIT_SUCCESS; 
}
// Método obtener los vecinos de la red hipercubo
void conocerVecinos(int rank, int* vecinos){
    int rank_aux;

    for(int i = 0; i < D; i++){
        rank_aux = 1 << i; //movemos el bit i posiciones
        vecinos[i] = rank ^ rank_aux; //hacemos un XOR con cada bit para obtener el vecino 
    } 
}

//Método para obtener el número máximo
double maximo(int rank, double bufferRank, int* vecinos){
    int i; 
    double Numero_maximo;  
    MPI_Status status; 
    MPI_Request request;

    for(i = 0; i < D; i++){ 
        if(bufferRank > Numero_maximo){  
            //comparo mi máximo con el valor actual del buffer
            Numero_maximo = bufferRank; 
        }
    
        MPI_Isend(&Numero_maximo, 1, MPI_DOUBLE, vecinos[i], i, MPI_COMM_WORLD,&request);  
        MPI_Wait(&request, &status);
        MPI_Recv(&bufferRank, 1, MPI_DOUBLE, vecinos[i], i, MPI_COMM_WORLD, &status);   
        
        if(bufferRank > Numero_maximo){
            //comparo mi máximo con el máximo del vecino 
            Numero_maximo = bufferRank; 
        }
    }
    return Numero_maximo; 
}

