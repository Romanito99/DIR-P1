/*Lectura del fichero datos.dat*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATOS "datos.dat"
#define MAX 1000

int leerFichero(double *numeros); 

int leerFichero(double *numeros){
	char *listaNumeros=malloc(MAX * sizeof(char));
	int cantidadNumeros=0;
	char *numeroActual;

	FILE *fichero=fopen(DATOS, "r");
	if(!fichero){
		fprintf(stderr,"ERROR: no se pudo abrir el fichero\n.");
		return 0;
	}
	fscanf(fichero, "%s", listaNumeros);
	fclose(fichero);
	numeros[cantidadNumeros++]=atof(strtok(listaNumeros,","));
	while( (numeroActual = strtok(NULL, "," )) != NULL ){
		numeros[cantidadNumeros++]=atof(numeroActual);
	}
	
	return cantidadNumeros;
}