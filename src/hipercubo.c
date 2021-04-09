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

void obtenerVecinos(int rank, int* vecinos); 
double obtenerNumeroMaximo(int rank, double bufferRank, int* vecinos);

int main(int argc, char *argv[]){
    int rank, size; 
    int numNodos = pow(2,D); //indica el numero de procesos que vamos a tener dependiendo de la dimension dada 
    double bufferRank; //buffer que almacena los rank
    int sigue = 0; //0 si sigue la ejecucion y 1 si no. 
    int vecinos[D]; //array de vecinos de nuestro nodo
    MPI_Status status; 
    MPI_Request request;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    //Si soy el rank 0
    if(rank == 0){
        
        if(numNodos != size){
            fprintf(stderr, "ERROR. Para un hipercubo de dimensión %d se deben lanzar %d procesos\n", D, numNodos); 
            
            //error
            sigue = 1; 
            MPI_Bcast(&sigue, 1, MPI_INT, 0, MPI_COMM_WORLD); 

        }else{ 

            
            double *numleidos; 
            numleidos = malloc(MAX_TAM * sizeof(double)); 

           
            int valFichero = leerFichero(numleidos); 

            
            if(numNodos != valFichero){
                fprintf(stderr, "ERROR. No hay el número correcto de valores en el fichero. Hacen falta %d valores\n", numNodos); 
            
                //error
                sigue = 1; 
                MPI_Bcast(&sigue, 1, MPI_INT, 0, MPI_COMM_WORLD); 

            }else{
                //Se continua con la ejecuion en el resto de nodos 
                MPI_Bcast(&sigue, 1, MPI_INT, 0, MPI_COMM_WORLD);

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
    MPI_Bcast(&sigue, 1, MPI_INT, 0, MPI_COMM_WORLD); 

    if(sigue == 0){
        //Recibo mi dato
        MPI_Recv(&bufferRank, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 
        obtenerVecinos(rank, vecinos); 
        double numMax = obtenerNumeroMaximo(rank, bufferRank, vecinos); 
    
        if(rank == 0){
            printf("[RANK %d] El valor máximo de la red es: %.2f\n", rank, numMax); 
        }
    }

    MPI_Finalize(); 

    return EXIT_SUCCESS; 
}


// Método obtener los vecinos de la red hipercubo
void obtenerVecinos(int rank, int* vecinos){
    int rank_aux;

    for(int i = 0; i < D; i++){
        rank_aux = 1 << i; //movemos el bit i posiciones
        vecinos[i] = rank ^ rank_aux; //hacemos un XOR con cada bit para obtener el vecino 
    } 
}

//Método para obtener el número máximo
double obtenerNumeroMaximo(int rank, double bufferRank, int* vecinos){
    int i; 
    double max; //este es el máximo de mi nodo que yo le enviaré a mi vecino /*¿CUAL ES EL MAXIMO?*/ 
    MPI_Status status; 
    MPI_Request request;

    for(i = 0; i < D; i++){ //como tantos vecinos tenga
        if(bufferRank > max){  
            //comparo mi máximo con el valor actual del buffer
            max = bufferRank; 
        }
    
        MPI_Isend(&max, 1, MPI_DOUBLE, vecinos[i], i, MPI_COMM_WORLD,&request); //envio mi maximo a mi vecino 
        MPI_Recv(&bufferRank, 1, MPI_DOUBLE, vecinos[i], i, MPI_COMM_WORLD, &status); // recibo el máximo de mi vecino  
        MPI_Wait(&request, &status);

        if(bufferRank > max){
            //comparo mi máximo con el máximo del vecino 
            max = bufferRank; 
        }
    }

    return max; 
}

