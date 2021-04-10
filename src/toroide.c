/*Red toroide*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "mpi.h"
#include "leerFichero.h"

#define L 4 //Lado de la red
#define MAX_TAM 1000
#define MAX 1000

void conocerVecinos(int rank, int *norte, int *sur, int *este, int *oeste);
double minimo(int rank, double bufferRank, int norte, int sur, int este, int oeste);

int main(int argc, char *argv[])
{

    int rank, size;
    double bufferRank;        //buffer que almacena los rank
    int error = 0;            //0  error 1 correcto.
    int numero_Nodos = L * L; //número de procesos 
    int norte, sur, este, oeste;

    MPI_Status status;
    MPI_Request request;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Si soy el rank 0
    if (rank == 0)
    {
        //Se comprueba el numero de procesos 
        if (numero_Nodos != size)
        {
            fprintf(stderr, "ERROR. Para un toroide de lado %d se deben lanzar %d procesos\n", L, numero_Nodos);

            //Se para la ejecucion 
            error = 1;
            MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);
        }
        else
        { 

            
            double *numleidos;
            numleidos = malloc(MAX_TAM * sizeof(double));

            //Leemos el fichero para obtener los valores
            int valoresFichero = leerFichero(numleidos);

            //Se comprueba que la cantidad de numeros en el fichero es correcta
            if (numero_Nodos != valoresFichero)
            {
                fprintf(stderr, "ERROR. No hay el número correcto de valores en el fichero. Se necesitan %d valores\n", numero_Nodos);

                //Se para la ejecucion 
                error = 1;
                MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);
            }
            else
            {
                //Se continua con la ejecucion en el resto de nodos
                MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);

                //El root reparte los valores del fichero para cada uno de los demas nodos
                for (int j = 0; j < valoresFichero; ++j)
                {
                    bufferRank = numleidos[j];
                    MPI_Isend(&bufferRank, 1, MPI_DOUBLE, j, 0, MPI_COMM_WORLD, &request);
                    MPI_Wait(&request, &status);
                }
            }
        }
    }

    //Esperamos que el root nos indique si podemos seguir con la ejecucion.En este MPI_Bcast , bloqueamos la ejecucion 
    MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (error == 0)
    {
        MPI_Recv(&bufferRank, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        conocerVecinos(rank, &norte, &sur, &este, &oeste);
        double numeroMinimo = minimo(rank, bufferRank, norte, sur, este, oeste);

        //El rank0 imprime el número menor de la red
        if (rank == 0)
        {
            printf("[RANK %d] El valor mínimo de la red es: %.2f\n", rank, numeroMinimo);
        }
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}
// Método para saber cuales son los vecinos de la red toroide
void conocerVecinos(int rank, int *norte, int *sur, int *este, int *oeste)
{
    int fila = rank / L;
    int columna = rank % L;

    //Para obtener los vecinos sur y norte
    switch (fila)
    {
    case 0: //0
        *sur = (L * (L - 1)) + columna;
        *norte = (L * (fila + 1)) + columna;
        break;

    case L - 1: //3
        *sur = (L * (fila - 1)) + columna;
        *norte = columna;
        break;

    default: //1-2
        *sur = (L * (fila - 1)) + columna;
        *norte = (L * (fila + 1)) + columna;
        break;
    }

    //Para obtener a los vecinos este y oeste
    switch (columna)
    {
    case 0: //0
        *este = (L * fila) + 1;
        *oeste = (L * fila) + (L - 1);
        break;

    case L - 1: //3
        *este = L * fila;
        *oeste = (L * fila) + (columna - 1);
        break;

    default: //1-2
        *este = (L * fila) + (columna + 1);
        *oeste = (L * fila) + (columna - 1);
        break;
    }
}

//Método para obtener el menor número
double minimo(int rank, double bufferRank, int norte, int sur, int este, int oeste)
{
    int i;
    double min;
    MPI_Status status;
    MPI_Request request;

    //Primero obtenemos los minimos de Norte a Sur (por filas)
    for (i = 0; i < L; i++)
    {
        if (bufferRank < min)
        { 
            min = bufferRank;
        }
        MPI_Isend(&min, 1, MPI_DOUBLE, sur, i, MPI_COMM_WORLD, &request); //envio mi numero al vecino sur
        MPI_Wait(&request, &status);
        MPI_Recv(&bufferRank, 1, MPI_DOUBLE, norte, i, MPI_COMM_WORLD, &status); //recibo el número de mi vecino norte

        if (bufferRank < min)
        {
            min = bufferRank;
        }
    }

    //Ahora obtengo los valores de mis vecinos de Este a Oeste (por columnas)
    for (i = 0; i < L; i++)
    {
        MPI_Isend(&min, 1, MPI_DOUBLE, este, i, MPI_COMM_WORLD, &request); //envio mi numero al vecino este
        MPI_Wait(&request, &status);
        MPI_Recv(&bufferRank, 1, MPI_DOUBLE, oeste, i, MPI_COMM_WORLD, &status); //recibo el numero de mi vecino oeste

        //obtengo el menor numero 
        if (bufferRank < min)
        {
            min = bufferRank;
        }
    }

    return min;
}