/*

Nombre:   Prieto Lozano Adrian
Código:   217761668
Materia:  Estructuras de datos II
Sección:  D09
Profesor: Guerrero Segura Ramírez Miguel Ángel

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "arches.h"
#include "tomarc.c"

#define LONG_REG   64
#define NUM_REG    50
#define NUM_CAMPOS 8
#define CONTENEDOR 3
#define campo_a_buffreg( br, cad ) strcat( br, cad ); strcat( br, DELIM_CAD );

int pedirRegistro( char buffer[] );
void pedirLlave( char cad[] );
void mostrarRegistro( const char buffer[], int longitud );
int opcionMenu( void );
void genera( int fd );
int dispersion( const char apellido[] );
int cadespac( char cad[] );
void mayusculas( char cad[] );
long encontrarLugar( int fd, char apellido[] );
int buscarEnContenedor( int fd, char clave[], char buffreg[], int *seEncontro );
long lseek();

char *camposAPedir[] = { "Apellido paterno (ENTER para salir): ",
						"Apellido materno: ",
						"Nombre: ",
						"Direccion: ",
						"Telefono: ",
						"Tipo de auto: ",
						"Marca del auto: ",
						"Placas del auto: " };

int main( void )
{
	char nomarch[ 12 ];
	char clave[ LONG_REG ];
	char buffreg[ TAM_MAX_REG + 1 ];
	char apellidoPaterno[ LONG_REG ];
	char caracter;
	int fd;
	int contadorReg, continuar, banderaEncontrado, numIteraciones, pos_bus;
	int longitud, menuEleccion, byte_pos, posIndice;

	printf( "Ingresa el nombre del archivo: " );
	gets( nomarch );

	fd = open( nomarch, LECTESCRIT ); // abre archivo de registros

	if( fd < 0 ) // si no se pudo abrir el archivo
	{
		if( ( fd = creat( nomarch, PMODE ) ) < 0 ) // intenta crear el archivo
		{
			printf("No se pudo abrir ni crear el archivo\n");
			exit( 1 );
		}
		else
			genera( fd );
	}

	while( ( menuEleccion = opcionMenu() ) != 3 )
	{
		switch( menuEleccion )
		{
			case 1:
				while( pedirRegistro( buffreg ) )
				{
					pos_bus = toma_campo( apellidoPaterno, buffreg, 0, strlen( buffreg ) ); // toma el apellido paterno
					byte_pos = encontrarLugar( fd, apellidoPaterno ); // busca un lugar y aumenta el contador
					if( byte_pos == -1 )
						printf("No hay espacio para el registro\n");
					else
					{
						lseek( fd, ( long )byte_pos, 0 );
						write( fd, buffreg, LONG_REG );
					}
				}
				break;

			case 2:
				pedirLlave( clave );
				banderaEncontrado = buscarYMostrar( fd, clave, buffreg );
				if( banderaEncontrado == 0 )
					printf("No se encontro el registro\n");
				break;
		}

	}
	lseek( fd, 0L, 0 );
	close( fd );

	return 0;
}

/* Prepara el archivo */
void genera( int fd )
{
	int contadorReg = 0;
	char buffer[ LONG_REG ];
	for( int i = 0; i < LONG_REG; buffer[ i++ ] = '\0' );

	for( int i = 0; i < NUM_REG; i++ )
	{
		write( fd, &contadorReg, sizeof( int ) );
		for( int j = 0; j < CONTENEDOR; j++ )
			write( fd, buffer, LONG_REG );
	}
}

/* Muestra el menú y devuelve la opción elegida */
int opcionMenu( void )
{
	char respuesta[ 2 ];
	int opcion;

	do
	{
		printf("\nSelecciona una opcion\n");
		printf("1. Nuevo registro\n");
		printf("2. Buscar registro\n");
		printf("3. Salir\n");

		gets( respuesta );
		opcion = atoi( respuesta );
	}while( opcion != 1 && opcion != 2 && opcion != 3 );

	return opcion;
}


/* Pide un registro campo por campo y lo coloca en buffer */
int pedirRegistro( char buffer[] )
{
	char respuesta[ LONG_REG ];

	for( int i = 0; i < LONG_REG; buffer[ i++ ] = '\0' );
	for( int i = 0; i < LONG_REG; respuesta[ i++ ] = '\0' );

	printf("\n%39s", camposAPedir[ 0 ]);
	gets( respuesta );
	cadespac( respuesta );
	if( strlen( respuesta ) > 0 )
	{ 
		campo_a_buffreg( buffer, respuesta );
		for( int i = 1; i < NUM_CAMPOS; i++ )
		{
			printf("%39s", camposAPedir[ i ]);
			gets( respuesta );
			cadespac( respuesta );
			campo_a_buffreg( buffer, respuesta );
		}
		return 1;
	}
	return 0;
}

/* Realiza el algoritmo de dispersión */
int dispersion( const char apellido[] )
{
    // llena la el sobrante de la llave con espacios
    char llaveCopia[ LONG_REG ];
    for( int i = 0; i < LONG_REG; llaveCopia[ i++ ] = '\0' );
    strcpy( llaveCopia, apellido );
    if( strlen( llaveCopia ) < 12 )
    {
        for( int i = strlen( llaveCopia ); i < 12; i++ )
            llaveCopia[ i ] = ' ';
    }
    llaveCopia[ strlen( llaveCopia ) ] = '\0';

    // realiza el algoritmo
    long sum = 0;
    int j = 0;
    while( j < 12 )
    {
        sum = ( sum + 100 * llaveCopia[ j ] + llaveCopia[ j + 1 ] ) % 20000;
        j += 2;
    }
    return ( sum % 47 ); // retorna la posición donde se guardará el registros.
}

/* Retorna la posición en bytes en el archivo donde hay una posición disponible
para insertar un nuevo registo */
 long encontrarLugar( int fd, char apellido[] )
 {
 	int contadorReg, posIndice, numIteraciones = 0;
 	long cpos, posByte;
 	char caracter;

 	cadespac( apellido );
 	mayusculas( apellido );
 	posIndice = dispersion( apellido );

 	do
 	{
 		posByte = ( LONG_REG * CONTENEDOR + sizeof( int ) ) * posIndice;// + sizeof( encabezado );
 		cpos = lseek( fd, ( long )posByte, 0 );
 		read( fd, &contadorReg, sizeof( int ) );
 		if( contadorReg < CONTENEDOR )
 		{
 			contadorReg++;
 			cpos = lseek( fd, ( long )posByte, 0 );
 			write( fd, &contadorReg, sizeof( int ) );
 			cpos = lseek( fd, 0L, 1 ); // obtiene la posicion actual
 			for( int i = 0; i < CONTENEDOR; i++ )
 			{
 				read( fd, &caracter, 1 );
 				if( caracter == '\0' )
 					return cpos;
 				lseek( fd, -1, 1 ); // se regresa una posición en el archivo (por el caracter leído)
 				cpos = lseek( fd, LONG_REG, 1 );
 			}
 		}
 		posIndice++;
 		numIteraciones += contadorReg;
 		// Si el numero de interaciones es igual al numero total de registros significa que el archivo esta lleno
 		if( numIteraciones == ( NUM_REG * CONTENEDOR ) )
 			return -1;
 		posIndice = ( posIndice == NUM_REG ? 0 : posIndice );
 	}while( 1 );

 }


/* Pide el apellido a buscar */
void pedirLlave( char cad[] )
{
	char respuesta[ LONG_REG ];
	for( int i = 0; i < LONG_REG; respuesta[ i++ ] = '\0' );

	do
	{
		printf("Apellido paterno: ");
		gets( respuesta );
	}
	while( strlen( respuesta ) == 0 );
	strcpy( cad, respuesta );
}

/* Muestra el registro contenido en buffer campo por campo */
void mostrarRegistro( const char buffer[], int longitud )
{
	char campo[ LONG_REG + 1 ];
	int cont_campos = 0;
	int pos_bus = 0;

	do
	{
		pos_bus = toma_campo( campo, buffer, pos_bus, longitud );
		if( pos_bus > 0 )
			printf("\tCampo %i: %s\n", ++cont_campos, campo);
	}while( pos_bus > 0 && ( cont_campos < NUM_CAMPOS ) );
}

/* Muestra los registros encontrados en el contenedor */
 int buscarEnContenedor( int fd, char clave[], char buffreg[], int *seEncontro )
 {
 	int longitud;
 	char apellido[ LONG_REG ];

	for( int i = 0; i < CONTENEDOR; i++ )
	{
		longitud = read( fd, buffreg, LONG_REG );
		toma_campo( apellido, buffreg, 0, longitud ); // toma el apellido paterno
		mayusculas( apellido );
		if( strcmp( clave, apellido ) == 0 )
		{
			printf("\n");
			mostrarRegistro( buffreg, longitud );
			*seEncontro = 1;
			printf("\n");
		}
		else if( apellido[ 0 ] == '\0' )
			return 0; // ya no continuar
	}
	return 1; // para seguir buscando en otro contenedor
 }

/* Elimina los espacios al final de la cadena */
int cadespac( char cad[] )
{
	int i;

	for( i = strlen( cad ) - 1; i >= 0 && cad[ i ] == ' '; i-- );

	/* Ahora que los espacios se han eliminado, se recoloca el nulo
	al final para formar la cadena */

	cad[ ++i ] = '\0';
	return i;
}

/* Convierte a mayusculas cadSal */
void mayusculas( char cadSal[] )
{
	int desp = 'a' - 'A';
	for( int i = 0; cadSal[ i ] != '\0'; i++ )
		if( cadSal[ i ] >= 'a' && cadSal[ i ] <= 'z' )
			cadSal[ i ] = cadSal[ i ] - desp;
}

int buscarYMostrar( int fd, char clave[], char buffreg[] )
{
	int numIteraciones = 0, continuar;
	int encontrado = 0, contadorReg;
	long byte_pos;

	cadespac( clave );
	mayusculas( clave );
	int posIndice = dispersion( clave );

	do
	{
		byte_pos = ( LONG_REG * CONTENEDOR + sizeof( int ) ) * posIndice;// + sizeof( encabezado );
		lseek( fd, ( long )byte_pos, 0 );
		read( fd, &contadorReg, sizeof( int ) );
		if( contadorReg > 0 )
			continuar = buscarEnContenedor( fd, clave, buffreg, &encontrado );
		else
			continuar = 0;

		posIndice++;
		posIndice = ( posIndice == NUM_REG ? 0 : posIndice );
		numIteraciones++;
		continuar = ( numIteraciones == NUM_REG ? 0 : 1 );

	}while( continuar );

	return encontrado;
}
