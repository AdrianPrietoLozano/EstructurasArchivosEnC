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
#define LIMITE     100
#define NUM_CAMPOS 8
#define campo_a_buffreg( br, cad ) strcat( br, cad ); strcat( br, DELIM_CAD );
#define escribe_sin_deli( br, cad ) strcat( br, cad );

int pedirRegistro( char buffer[] );
void pedirLlave( char cad[] );
void mostrarRegistro( const char buffer[], int longitud );
int opcionMenu( void );
void genera( int fd );
int dispersion( const char apellido[] );
int cadespac( char cad[] );
void mayusculas( char cad[] );
long encontrarLugar( int fd, char apellido[] );
int buscarYMostrar( int fd, char clave[], char buffreg[] );

// encabezado para el archivo de datos
struct Encabezado
{
	short cont_reg;
	char  relleno[ 30 ];
} encabezado;

long lseek();

int main( void )
{
	int menuEleccion, pos_bus, byte_pos, banderaEncontrado, numIteraciones;
	int posIndice, longitud, fd;
	char nomarch[ 15 ];
	char clave[ LONG_REG ];
	char buffreg[ TAM_MAX_REG + 1 ];
	char apellidoPaterno[ LONG_REG ];
	char caracter;

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
		{
			encabezado.cont_reg = 0;
			write( fd, &encabezado, sizeof( encabezado ) );
			genera( fd );
		}
	}
	else
		read( fd, &encabezado, sizeof( encabezado ) );


	while( ( menuEleccion = opcionMenu() ) != 3 )
	{
		switch( menuEleccion )
		{
			case 1:
				while( encabezado.cont_reg < LIMITE && pedirRegistro( buffreg ) )
				{
					toma_campo( apellidoPaterno, buffreg, 0, strlen( buffreg ) ); // toma el apellido paterno
					byte_pos = encontrarLugar( fd, apellidoPaterno ); // busca un lugar y aumenta el contador
					lseek( fd, ( long )byte_pos, 0 );
					write( fd, buffreg, LONG_REG );
					encabezado.cont_reg++;
				}
				if( encabezado.cont_reg > LIMITE )
					printf("Haz alcanzado el limite\n");
				break;

			case 2:
				//for( int i = 0; i < sizeof( apellidoPaterno ); apellidoPaterno[ i++ ] = '\0' );
				pedirLlave( clave );
				banderaEncontrado = buscarYMostrar( fd, clave, buffreg );
				if( banderaEncontrado == 0  )
					printf(" El registro no existe\n");
				break;
		}

	}
	lseek( fd, 0L, 0 );
	write( fd, &encabezado, sizeof( encabezado ) );

	close( fd );

	return 0;
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

/* Pide un registro campo por campo y lo coloca en buffer */
int pedirRegistro( char buffer[] )
{
	char respuesta[ LONG_REG ];

	for( int i = 0; i < LONG_REG; buffer[ i++ ] = '\0' );
	for( int i = 0; i < LONG_REG; respuesta[ i++ ] = '\0' );

	printf("\n%39s", "Apellido paterno (ENTER para salir): ");
	gets( respuesta );
	cadespac( respuesta );

	while( strlen( respuesta ) > 0 )
	{
		campo_a_buffreg( buffer, respuesta );

		printf("%39s", "Apellido materno: ");
		gets( respuesta );
		cadespac( respuesta );
		if( strlen( buffer ) < LONG_REG )
			campo_a_buffreg( buffer, respuesta );

		printf("%39s", "Nombre: ");
		gets( respuesta );
		cadespac( respuesta );
		if( strlen( buffer ) < LONG_REG )
			campo_a_buffreg( buffer, respuesta );

		printf("%39s", "Direccion: ");
		gets( respuesta );
		cadespac( respuesta );
		if( strlen( buffer ) < LONG_REG )
			campo_a_buffreg( buffer, respuesta );

		printf("%39s", "Telefono: ");
		gets( respuesta );
		cadespac( respuesta );
		if( strlen( buffer ) < LONG_REG )
			campo_a_buffreg( buffer, respuesta );

		printf("%39s", "Tipo de auto: ");
		gets( respuesta );
		cadespac( respuesta );
		if( strlen( buffer ) < LONG_REG )
			campo_a_buffreg( buffer, respuesta );

		printf("%39s", "Marca del auto: ");
		gets( respuesta );
		cadespac( respuesta );
		if( strlen( buffer ) < LONG_REG )
			campo_a_buffreg( buffer, respuesta );

		printf("%39s", "Placas del auto: ");
		gets( respuesta );
		cadespac( respuesta );
		if( strlen( buffer ) < LONG_REG )
			campo_a_buffreg( buffer, respuesta );

		buffer[ strlen( buffer ) ] = '\0';
		
		return 1;
	}
	return 0;
}

/* Muestra el menú y devuelve la opción elegida */
int opcionMenu( void )
{
	char respuesta[ 5 ];
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

/* Muestra el registro contenido en buffer campo por campo */
void mostrarRegistro( const char buffer[], int longitud )
{
	char campo[ LONG_REG + 1 ];
	int cont_campos = 0;
	int pos_bus = 0;

	while( cont_campos != NUM_CAMPOS )
	{
		pos_bus = toma_campo( campo, buffer, pos_bus, longitud );
		if( pos_bus > -1 )
			printf("\tCampo %i: %s\n", ++cont_campos, campo);
		else
			printf("\tCampo %i:\n", ++cont_campos);
	}
}

void genera( int fd )
{
	char buffer[ LONG_REG ];
	for( int i = 0; i < LONG_REG; buffer[ i++ ] = '\0' );

	for( int i = 0; i < LIMITE; i++ )
		write( fd, buffer, LONG_REG );
}

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
        sum = ( sum + 100 * llaveCopia[ j ] + llaveCopia[ j + 1 ] )  % 20000;
        j += 2;
    }
    return ( sum % 99 ); // retorna la posición donde se guardará el registros.
}

int cadespac( char cad[] )
{
	int i;

	for( i = strlen( cad ) - 1; i >= 0 && cad[ i ] == ' '; i-- );

	/* Ahora que los espacios se han eliminado, se recoloca el nulo
	al final para formar la cadena */

	cad[ ++i ] = '\0';
	return i;
}

void mayusculas( char cadSal[] )
{
	int desp = 'a' - 'A';
	for( int i = 0; cadSal[ i ] != '\0'; i++ )
		if( cadSal[ i ] >= 'a' && cadSal[ i ] <= 'z' )
			cadSal[ i ] = cadSal[ i ] - desp;
 }


 long encontrarLugar( int fd, char apellido[] )
 {
 	int posIndice;
 	long cpos, posByte;
 	char caracter;

 	cadespac( apellido );
	mayusculas( apellido );
	posIndice = dispersion( apellido );

	do
	{
		posByte = LONG_REG * posIndice + sizeof( encabezado );
		cpos = lseek( fd, ( long )posByte, 0 );
		read( fd, &caracter, 1 );
		posIndice++;
		posIndice = ( posIndice == LIMITE ? 0 : posIndice );
	} while( caracter != '\0' );

	return cpos;
 }


 int buscarYMostrar( int fd, char clave[], char buffreg[] )
 {
 	int encontrado = 0, numIteraciones = 0;
 	int posIndice, byte_pos, longitud;
 	char apellidoPaterno[ LONG_REG ];

 	cadespac( clave );
	mayusculas( clave );
	posIndice = dispersion( clave );
	do
	{
		byte_pos = LONG_REG * posIndice + sizeof( encabezado );
		lseek( fd, ( long )byte_pos, 0 );
		longitud = read( fd, buffreg, LONG_REG );
		toma_campo( apellidoPaterno, buffreg, 0, longitud ); // toma el apellido paterno
		cadespac( apellidoPaterno );
		mayusculas( apellidoPaterno );
		if( strcmp( clave, apellidoPaterno ) == 0 )
		{
			encontrado = 1;
			printf("\n");
			mostrarRegistro( buffreg, longitud );
			printf("\n");
		}
		posIndice++;
		posIndice = ( posIndice == LIMITE ? 0 : posIndice );
		numIteraciones++;

	}while( apellidoPaterno[ 0 ] != '\0' && ( numIteraciones < LIMITE ) );

	return encontrado;
 }