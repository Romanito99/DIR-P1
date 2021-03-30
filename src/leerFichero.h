/*Lectura del fichero datos.dat*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATOS "datos.dat"
#define MAX 1000

int leerFichero(double *numeros); 
//le pasamos el array numleidos que devolverá los números del fichero y el entero numeros que es el tamaño del array
int leerFichero(double *numeros){

	//Vector auxiliar de char para trabajar con los numeros del fichero
	char *listaNumeros=malloc(MAX * sizeof(char));

	//Tamanio del vector de numeros
	int cantidadNumeros=0;

	//Caracter auxiliar para trabajar con los numeros del fichero
	char *numeroActual;

	//Abrimos el fichero con permisos de lectura
	FILE *fichero=fopen(DATOS, "r");
	if(!fichero){
		fprintf(stderr,"ERROR: no se pudo abrir el fichero\n.");
		return 0;
	}

	//Copiamos los datos del fichero al vector auxiliar de char
	fscanf(fichero, "%s", listaNumeros);

	//Cerramos el fichero
	fclose(fichero);

	//Leemos el primer numero hasta la primera coma. Usamos la funcion strtok. Con atof transformamos el string a double
	numeros[cantidadNumeros++]=atof(strtok(listaNumeros,","));

	//Vamos leyendo hasta que no haya mas numeros delante de las comas
	while( (numeroActual = strtok(NULL, "," )) != NULL ){
		//Metemos en el vector el numero correspondiente
		numeros[cantidadNumeros++]=atof(numeroActual);
	}
	
	return cantidadNumeros;
}