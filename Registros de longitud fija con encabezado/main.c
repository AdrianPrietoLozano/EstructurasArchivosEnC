#include "arches.h"
#include "tomarc.c"

#define LONG_REG   64
#define LIMITE     2000
#define NUM_CAMPOS 6
#define campo_a_buffreg( br, cad ) strcat( br, cad ); strcat( br, DELIM_CAD );

static int fd;

int opcionMenu( void );
int pedirRegistro( char buffer[] );
int pedirNrr( void );
void mostrarRegistro( char buffer[] );
void estableceCampo( char campo[], int longitud );

struct Encabezado
{
	short cont_reg;
	char  relleno[ 30 ];
} encabezado;


int main( void )
{
	int menuEleccion, nrr;
	int byte_pos;
	char nomarch[ 15 ];
	long lseek();
	char buffreg[ TAM_MAX_REG + 1 ]; // buffer para guardar un registro

	printf( "Ingresa el nombre del archivo: " );
	gets( nomarch );

	if( ( fd = open( nomarch, LECTESCRIT ) ) < 0 )
		if( ( fd = creat( nomarch, PMODE ) ) < 0 )
		{
			printf("No se pudo abrir ni crear el archivo\n");
			exit( 1 );
		}
		else
		{
			encabezado.cont_reg = 0;
			write( fd, &encabezado, sizeof( encabezado ) );
		}
	else
		read( fd, &encabezado, sizeof( encabezado ) );

	while( ( menuEleccion = opcionMenu() ) != 3 )
	{
		switch( menuEleccion )
		{
			case 1:
				while( !(encabezado.cont_reg > LIMITE) && pedirRegistro( buffreg ) )
				{
					byte_pos = LONG_REG * encabezado.cont_reg + sizeof( encabezado );
					lseek( fd, ( long )byte_pos, 0 );
					write( fd, buffreg, LONG_REG );
					encabezado.cont_reg++;
				}
				if( encabezado.cont_reg > LIMITE )
					printf("Haz alcanzado el limite\n");
				break;

			case 2:
				if( encabezado.cont_reg > 0 )
				{
					nrr = pedirNrr();
					byte_pos = nrr * LONG_REG + sizeof( encabezado );
					lseek( fd, ( long )byte_pos, 0 );
					read( fd, buffreg, LONG_REG );
					printf("\nRegistro con NRR %i:\n", nrr);
					mostrarRegistro( buffreg );
				}
				else
					printf("No hay registros\n");
				break;
		}
		lseek( fd, 0L, 0 );
		write( fd, &encabezado, sizeof( encabezado ) );
	}

	return 0;
}


int opcionMenu( void )
{
	char respuesta[ 5 ];
	int opcion;

	do
	{
		printf("\nSelecciona una opcion\n");
		printf("1. Nuevo registro\n");
		printf("2. Mostrar registro\n");
		printf("3. Salir\n");

		gets( respuesta );
		opcion = atoi( respuesta );
	}while( opcion != 1 && opcion != 2 && opcion != 3 );

	return opcion;
}

int pedirRegistro( char buffer[] )
{
	char respuesta[ 50 ];
	char tiempo[ 10 ];
	char b[ 10 ];

	for( int i = 0; i < LONG_REG; buffer[ i++ ] = '\0' );
	for( int i = 0; i < 7; tiempo[ i++ ] = '\0' );	

	printf("\n Ingresa el numero de participacion o <CR> para terminar: ");
	gets( respuesta );

	while( strlen( respuesta ) > 0 )
	{
		estableceCampo( respuesta, 4 );
		campo_a_buffreg( buffer, respuesta );

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
		campo_a_buffreg( buffer, tiempo );

		printf("\n Nombre del atleta: ");
		gets( respuesta );
		estableceCampo( respuesta, 18 );
		campo_a_buffreg( buffer, respuesta );

		printf(" Direccion del atleta: ");
		gets( respuesta );
		estableceCampo( respuesta, 16 );
		campo_a_buffreg( buffer, respuesta );

		printf(" Ciudad: ");
		gets( respuesta );
		estableceCampo( respuesta, 10 );
		campo_a_buffreg( buffer, respuesta );

		printf(" Nacionalidad: ");
		gets( respuesta );
		estableceCampo( respuesta, 10 );
		campo_a_buffreg( buffer, respuesta );
		
		return 1;
	}
	return 0;
}

int pedirNrr( void )
{
	char respuesta[ 4 ];
	int eleccion;

	printf("Ingresa el NRR del registro: ");
	gets( respuesta );

	while( ( eleccion = atoi( respuesta ) ) > ( encabezado.cont_reg - 1 ) || eleccion < 0 )
	{
		printf("NRR no valido, intentalo de nuevo: ");
		gets( respuesta );
	}

	return eleccion;
}


void mostrarRegistro( char buffer[] )
{
	char campo[ LONG_REG + 1 ];
	int cont_campos = 0;
	int pos_bus = 0;
	int long_reg = strlen( buffer );

	while( ( pos_bus = toma_campo( campo, buffer, pos_bus, long_reg, cont_campos + 1 ) ) > 0 )
		printf("\tCampo %i: %s\n", ++cont_campos, campo);
}

void estableceCampo( char campo[], int longitud )
{
	int tam = strlen( campo );
	tam = ( tam < longitud ? tam : longitud - 1 );

	for( int i = 0; i < tam; i++ )
		campo[ i ] = campo [ i ];

	campo[ tam + 1 ] = '\0';
}