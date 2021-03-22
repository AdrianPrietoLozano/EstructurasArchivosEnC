/*

Nombre:   Prieto Lozano Adrian
Código:   217761668
Materia:  Estructuras de datos II
Sección:  D09
Profesor: Guerrero Segura Ramírez, Miguel Ángel

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "arches.h"
#include "tomarc.c"
#include "ordenar.c"
#include "busqueda.c"

#define OL -1

#define LONG_REG   64
#define LIMITE     2000
#define NUM_CAMPOS 6
#define campo_a_buffreg( br, cad ) strcat( br, cad ); strcat( br, DELIM_CAD );
#define escribe_sin_deli( br, cad ) strcat( br, cad );

typedef char REGDATOS[ LONG_REG + 1 ];
typedef char TIEMPO[ 10 ];
typedef char NUMPARTI[ 5 ];

static int fd;
static int fdIndice;

REGDATOS *registros;
TIEMPO *tiempo;
NUMPARTI *numParti;
int *arregloNrr;
int *arregloIndices;

int pedirRegistro( char buffer[] );
void pedirNum( char cad[] );
void mostrarRegistro( const char buffer[], int longitud );
int opcionMenu( void );
void mostrarTiempo( TIEMPO tiempo );


// encabezado para el archivo de datos
struct Encabezado
{
	short cont_reg;
	short bandera;
	char  relleno[ 30 ];
} encabezado;

// encabezado para el archivo de indices
struct encabezadoIndice
{
	short bandera;
} encaIndice;

struct Indice
{
	NUMPARTI numParti;
	int nrr;
	TIEMPO tiempo;
} indice;


int main( void )
{
	int menuEleccion;
	int byte_pos;
	int posIndice = 0; // la siguiente posición donde se deben agregar los datos a los arreglos dinámicos
	char nomarch[ 15 ];
	char nomIndice[ 20 ];
	long lseek();
	//char *calloc();
	char clave[ 10 ]; // para el número de participación a buscar
	int indiceClave;
	char buffreg[ TAM_MAX_REG + 1 ];
	char b[ 10 ];
	int longitud;
	char var[ 7 ];

	printf( "Ingresa el nombre del archivo: " );
	gets( nomarch );

	fd = open( nomarch, LECTESCRIT ); // abre archivo de registros
	strcpy( nomIndice, nomarch );
	strcat( nomIndice, "Ind" );
	fdIndice = open( nomIndice, LECTESCRIT ); // abre archivo de indices

	if( fd < 0 )
	{
		if( ( fd = creat( nomarch, PMODE ) ) < 0 )
		{
			printf("No se pudo abrir ni crear el archivo\n");
			exit( 1 );
		}
		else
		{
			encabezado.bandera = 0;
			encabezado.cont_reg = 0;
			write( fd, &encabezado, sizeof( encabezado ) );
		}
	}
	else
		read( fd, &encabezado, sizeof( encabezado ) );

	// Índice
	if( fdIndice < 0 )
	{
		if( ( fdIndice = creat( nomIndice, PMODE ) ) < 0 )
		{
			printf("No se pudo abrir/crear el archivo\n");
			exit( 1 );
		}
		else
		{
			encaIndice.bandera = 0;
			write( fdIndice, &encaIndice, sizeof( encaIndice ) );
		}
	}
	else
		read( fdIndice, &encaIndice, sizeof( encaIndice ) );

	// pide memoria para los arreglos dinámicos
	registros = ( REGDATOS * ) calloc( LIMITE, sizeof( REGDATOS ) );
	tiempo = ( TIEMPO * ) calloc( LIMITE, sizeof( TIEMPO ) );
	numParti = ( NUMPARTI * ) calloc( LIMITE, sizeof( NUMPARTI ) );
	arregloNrr = ( int * ) calloc( LIMITE, sizeof( int ) );
	arregloIndices = ( int * ) calloc( LIMITE, sizeof( int ) );

	/* Revisa que todas las asignaciones hayan sido exitosas */
	if( numParti == OL || arregloNrr == OL || tiempo == OL || registros == OL )
	{
		printf("No se pudo asignar el espacio requerido\n");
		printf("El archivo es demasiado grande para cargarse en memoria\n");
		exit( 1 );
	}

	for( int i = 0; i < LIMITE; i++ )
		*tiempo[ i ] = '\0';

	int pos_bus = 0;
	TIEMPO auxTiempo;
	NUMPARTI auxNumParti;

	/* Lee los registros secuencialmente, colocándolos en los arreglos
	y extrae las llaves y los otros campos */
	posIndice = 0;
	// si la bandera esta activada se recontruye el índice
	if( encaIndice.bandera == 1 || encabezado.bandera == 1 )
	{
		printf("RECONSTRUYENDO EL INDICE...\n");
		encabezado.cont_reg = 0;
		lseek( fd, ( long )sizeof( encabezado ), 0 );
		while( read( fd, registros[ posIndice ], LONG_REG ) > 0 )
		{
			pos_bus = 0;
			pos_bus = toma_campo( auxNumParti, registros[ posIndice ], pos_bus, LONG_REG, 1 );
			pos_bus = toma_campo( auxTiempo, registros[ posIndice ], pos_bus, LONG_REG, 2 );
			arregloNrr[ posIndice ] = posIndice;
			arregloIndices[ posIndice ] = posIndice;
			strcpy( numParti[ posIndice ], auxNumParti );
			strcpy( tiempo[ posIndice ], auxTiempo );
			posIndice++;
			encabezado.cont_reg++;
		}
		encaIndice.bandera = encabezado.bandera = 0; // apaga las banderas
		lseek( fd, 0L, 0 );
		write( fd, &encabezado, sizeof( encabezado ) );
		lseek( fdIndice, 0L, 0 );
		write( fdIndice, &encaIndice, sizeof( encaIndice ) );
		ordenar( arregloIndices, numParti, encabezado.cont_reg );
	}
	else // si las banderas estan apagadas simplemente lee el archivo donde esta el índice
	{	 // no es necesario leer el archivo de datos
		lseek( fdIndice, ( long )sizeof( encaIndice ), 0 );
		while( read( fdIndice, &indice, sizeof( indice ) ) > 0 )
		{
			strcpy( numParti[ posIndice ], indice.numParti );
			arregloNrr[ posIndice ] = indice.nrr;
			strcpy( tiempo[ posIndice ], indice.tiempo );
			arregloIndices[ posIndice ] = posIndice;
			posIndice++;
		}
	}


	while( ( menuEleccion = opcionMenu() ) != 4 )
	{
		switch( menuEleccion )
		{
			case 1:
				while( !( encabezado.cont_reg > LIMITE ) && pedirRegistro( buffreg ) )
				{
					byte_pos = LONG_REG * encabezado.cont_reg + sizeof( encabezado );
					lseek( fd, ( long )byte_pos, 0 );
					write( fd, buffreg, LONG_REG );
					encabezado.cont_reg++;
					if( encaIndice.bandera == 0 ) // activa al bandera del índice y la escribe
					{
						encaIndice.bandera = 1;
						lseek( fdIndice, 0L, 0 );
						write( fdIndice, &encaIndice, sizeof( encaIndice ) );
					}
					if( encabezado.bandera == 0 ) // activa la bandera del archivo de datos y la escribe
					{
						encabezado.bandera = 1;
						lseek( fd, 0L, 0 );
						write( fd, &encabezado, sizeof( encabezado ) );

					}

					pos_bus = 0;
					memcpy( registros[ posIndice ], buffreg, LONG_REG );
					pos_bus = toma_campo( auxNumParti, buffreg, pos_bus, strlen( buffreg ), 1 ); // toma el número de participación desde el arreglo dinámico
					pos_bus = toma_campo( auxTiempo, buffreg, pos_bus, strlen( buffreg ), 2 ); // toma el tiempo desde el arreglo dinámico

					strcpy( numParti[ posIndice ], auxNumParti ); // copia el num. participación al arreglo dinámico
					arregloNrr[ posIndice ] = posIndice; // agrega el nrr al arreglo de nrr´s
					strcpy( tiempo[ posIndice ], "" ); // copia el tiempo al arreglo dinámico
	
					arregloIndices[ posIndice ] = posIndice;
					ordenar( arregloIndices, numParti, encabezado.cont_reg ); // ordenar el arreglo nrr
					posIndice++;
				}
				if( encabezado.cont_reg > LIMITE )
					printf("Haz alcanzado el limite\n");
				break;

			case 2:
				if( encabezado.cont_reg > 0 )
				{
					pedirNum( clave );
					indiceClave = busquedaBinaria( arregloIndices, numParti, clave, encabezado.cont_reg );
					if( indiceClave != -1 )
					{

						byte_pos = arregloNrr[ indiceClave ] * LONG_REG + sizeof( encabezado );
						lseek( fd, ( long )byte_pos, 0 );
						longitud = read( fd, buffreg, LONG_REG );
						mostrarRegistro( buffreg, longitud );
						if( !(*tiempo[ indiceClave ]) == '\0' )
							mostrarTiempo(tiempo[ indiceClave ]);
						else
							printf("\tTiempo en esta carrera:\n");
					}
					else
						printf("No se encontro el registro\n");
				}
				else
					printf("No hay registros\n");
				break;
			case 3:
				pedirNum( clave );
				indiceClave = busquedaBinaria( arregloIndices, numParti, clave, encabezado.cont_reg );
				if( indiceClave != -1 )
				{
					for( int i = 0; i < strlen( b ); b[ i++ ] = '\0' );
					for( int i = 0; i < strlen( var ); var[ i++ ] = '\0' );
					printf(" Tiempo de clasificacion\n");
					printf("\t\tHoras(2 digitos): ");
					gets( b );
					while( strlen( b ) != 2 )
					{
						printf("Error. Ingresa solo dos digitos. Horas: ");
						gets( b );
					}
					strcat( var, b );
					printf("\t\tMinutos(2 digitos): ");
					gets( b );
					while( strlen( b ) != 2 )
					{
						printf("Error. Ingresa solo dos digitos. Minutos: ");
						gets( b );
					}
					strcat( var, b );
					printf("\t\tSegundos(2 digitos): ");
					gets( b );
					while( strlen( b ) != 2 )
					{
						printf("Error. Ingresa solo dos digitos. Segundos: ");
						gets( b );
					}
					strcat( var, b );
					byte_pos = indiceClave * sizeof( indice ) + sizeof( encaIndice );
					lseek( fdIndice, ( long )byte_pos, 0 );
					read( fdIndice, &indice, sizeof( indice ) );
					memcpy( indice.tiempo, var, 6 );
					memcpy( tiempo[ indiceClave ], var, 6 );
					lseek( fdIndice, ( long )byte_pos, 0 );
					write( fdIndice, &indice, sizeof( indice ) );
				}
				else
					printf("No se encontro el registro\n");
				break;
		}

	}
	encabezado.bandera = 0; // apaga la bandera del archivo de datos ya que no se va a usar y la escribe
	lseek( fd, 0L, 0 );
	write( fd, &encabezado, sizeof( encabezado ) );

	// Escribe el indice. Si esta la opción de eliminar entonces es necesario abrir un
	// archivo de indices nuevo y en ese escribir el índice, después eliminar el obsoleto.
	for( int i = 0; i < encabezado.cont_reg; i++ )
	{
		strcpy( indice.numParti, numParti[ arregloIndices[ i ] ] );
		indice.nrr = arregloNrr[ arregloIndices[ i ] ];

		if( !(*tiempo[ arregloIndices[ i ] ] == '\0') )
			strcpy( indice.tiempo, tiempo[ arregloIndices[ i ] ] );
		else
			for( int i = 0; i < sizeof( indice.tiempo ); indice.tiempo[ i++ ] = '\0' );
		
		lseek( fdIndice, ( long )( i * sizeof( indice ) + sizeof( encaIndice ) ), 0 );
		write( fdIndice, &indice, sizeof( indice ) );
	}
	encaIndice.bandera = 0; // apagar la bandera del índice y la escribe
	lseek( fdIndice, 0L, 0 );
	write( fdIndice, &encaIndice, sizeof( encaIndice ) );

	close( fd );
	close( fdIndice );

	return 0;
}

/* Pide el número de participación a buscar */
void pedirNum( char cad[] )
{
	char respuesta[ 50 ];

	printf("Numero de participacion: ");
	gets( respuesta );
	memcpy( cad, respuesta, 4 );
	cad[ strlen( cad ) ] = '\0';
}

/* Pide un registro campo por campo y lo coloca en buffer */
int pedirRegistro( char buffer[] )
{
	char respuesta[ 50 ];
	char tiempo[ 10 ];
	char b[ 10 ];

	for( int i = 0; i < LONG_REG; buffer[ i++ ] = '\0' );
	for( int i = 0; i < 7; tiempo[ i++ ] = '\0' );	

	printf("\n Ingresa el numero de participacion o <CR> para terminar: ");
	gets( respuesta );
	while( ( strlen( respuesta ) != 4 && strlen( respuesta ) > 0 ) || busquedaBinaria( arregloIndices, numParti, respuesta, encabezado.cont_reg ) != -1 )
	{
		printf(" Error. El numero ya existe o la entrada es incorrecta.\n Numero de participacion: ");
		gets( respuesta );
	}

	while( strlen( respuesta ) > 0 )
	{
		escribe_sin_deli( buffer, respuesta );

		printf(" Tiempo de clasificacion\n");
		printf("\t\tHoras(2 digitos): ");
		gets( b );
		while( strlen( b ) != 2 )
		{
			printf("Error. Ingresa solo dos digitos. Horas: ");
			gets( b );
		}
		strcat( tiempo, b );
		printf("\t\tMinutos(2 digitos): ");
		gets( b );
		while( strlen( b ) != 2 )
		{
			printf("Error. Ingresa solo dos digitos. Minutos: ");
			gets( b );
		}
		strcat( tiempo, b );
		printf("\t\tSegundos(2 digitos): ");
		gets( b );
		while( strlen( b ) != 2 )
		{
			printf("Error. Ingresa solo dos digitos. Segundos: ");
			gets( b );
		}
		strcat( tiempo, b );
		escribe_sin_deli( buffer, tiempo );

		printf("\n Nombre del atleta: ");
		gets( respuesta );
		campo_a_buffreg( buffer, respuesta );

		printf(" Ciudad: ");
		gets( respuesta );
		if( strlen( buffer ) < LONG_REG )
			campo_a_buffreg( buffer, respuesta );

		printf(" Nacionalidad: ");
		gets( respuesta );
		if( strlen( buffer ) < LONG_REG )
			campo_a_buffreg( buffer, respuesta );

		printf(" Direccion del atleta: ");
		gets( respuesta );
		if( strlen( buffer ) < LONG_REG )
			campo_a_buffreg( buffer, respuesta );

		buffer[ strlen( buffer ) ] = '\0';
		
		return 1;
	}
	return 0;
}

/* Muestra el menú y devuelve la opcioón elegida */
int opcionMenu( void )
{
	char respuesta[ 5 ];
	int opcion;

	do
	{
		printf("\nSelecciona una opcion\n");
		printf("1. Nuevo registro\n");
		printf("2. Mostrar registro\n");
		printf("3. Registrar tiempos\n");
		printf("4. Salir\n");

		gets( respuesta );
		opcion = atoi( respuesta );
	}while( opcion != 1 && opcion != 2 && opcion != 3 && opcion != 4 );

	return opcion;
}

/* Muestra el registro contenido en buffer campo por campo */
void mostrarRegistro( const char buffer[], int longitud )
{
	char campo[ LONG_REG + 1 ];
	int cont_campos = 0;
	int pos_bus = 0;
	//int tam_reg = strlen( buffer );

	while( cont_campos != NUM_CAMPOS )
	{
		pos_bus = toma_campo( campo, buffer, pos_bus, longitud, ++cont_campos );
		if( pos_bus > 0 )
			printf("\tCampo %i: %s\n", cont_campos, campo);
		else
			printf("\tCampo %i:\n", cont_campos);
	}
}

void mostrarTiempo( TIEMPO tiempo )
{
	printf("\tTiempo en esta carrera: ");
	printf("%c%c:%c%c:%c%c\n", tiempo[ 0 ], tiempo[ 1 ], tiempo[ 2 ], tiempo[ 3 ], tiempo[ 4 ], tiempo[ 5 ]);
}