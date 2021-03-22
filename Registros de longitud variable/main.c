/*

Nombre:  Prieto Lozano Adrian
Código:  217761668
Materia: Estructuras de datos II
Sección: D09

*/



#include "tomarc.c"
#define campo_a_buffreg( br, cad ) strcat( br, cad ); strcat( br, DELIM_CAD );


void escribir( void );
void leer( void );
int toma_reg( int fd, char buffreg[] );
int toma_campo( char campo[], char buffreg[], short pos_bus, short long_reg );

char buffreg[ TAM_MAX_REG + 1 ];
char nomArch[ 15 ];

int main( void )
{
    int opcion;

    printf("Escriba el nombre del archivo a crear o abrir: ");
	gets( nomArch ); // se obtiene el nombre del archivo físico

	if( ( open( nomArch, SOLOLEC ) ) < 0 )
    {
        if( ( creat( nomArch, PMODE ) ) < 0 )
        {
            printf("No se pudo abrir el archivo -- Fin del programa\n");
            exit( 1 );
        }
    }

    do
    {
    	printf("\nElige una opcion\n");
    	printf("1) Nuevo registro\n");
    	printf("2) Mostrar todo\n");
    	printf("3) Salir\n");
    	scanf( "%i", &opcion );
    	switch( opcion )
    	{
    		case 1:
                escribir();
                break;

    		case 2:
                leer();
                break;
    	}

    }while( opcion != 3 );

    return 0;
}


void escribir( void )
{
	char respuesta[ 50 ];
	int fd, i;
	int long_reg;

	if( ( fd = open( nomArch, SOLOESCRIT ) ) < 0 )
		printf("No se pudo abrir el archivo\n");
	else
	{
		printf("\nEscriba el titulo de la pelicula <CR> para salir: ");
		gets( respuesta );
		gets( respuesta );

		while( strlen( respuesta ) > 0 )
		{
			buffreg[ 0 ] = '\0';
			lseek( fd, 0L, 2 );
			campo_a_buffreg( buffreg, respuesta );
			printf("Protagonistas: ");
			gets( respuesta );
            campo_a_buffreg( buffreg, respuesta );
			printf("Director: ");
			gets( respuesta );
            campo_a_buffreg( buffreg, respuesta );
			printf("Estudio o Distribuidora: ");
			gets( respuesta );
            campo_a_buffreg( buffreg, respuesta );
			printf("Genero: ");
			gets( respuesta );
            campo_a_buffreg( buffreg, respuesta );
			printf("Anio de estreno: ");
			gets( respuesta );
            campo_a_buffreg( buffreg, respuesta );
			printf("Numero de premios: ");
			gets( respuesta );
            campo_a_buffreg( buffreg, respuesta );
			printf("Calificacion otorgada a la cinta: ");
			gets( respuesta );
            campo_a_buffreg( buffreg, respuesta );

            long_reg = strlen( buffreg );
		    write( fd, &long_reg, 2 );
		    write( fd, buffreg, long_reg );

		    printf("\nEscriba el titulo de la pelicula <CR> para salir: ");
		    gets( respuesta );
		} // fin while
	} // fin else

	close( fd );
}


void leer( void )
{
	int fd, cont_reg, cont_campos;
	int pos_bus, long_reg;
	char campo[ TAM_MAX_REG + 1 ];

	if( ( fd = open( nomArch, SOLOLEC ) ) < 0 )
		printf("No se pudo abrir el archivo\n");
	else
	{
		cont_reg = 0;
		pos_bus = 0;

		while( ( long_reg = toma_reg( fd, buffreg ) ) > 0 )
		{
			printf("\nRegistro %i\n", ++cont_reg);
			cont_campos = 0;
			while( ( pos_bus = toma_campo( campo, buffreg, pos_bus, long_reg ) ) > 0 )
				printf("\tCampo %i: %s\n", ++cont_campos, campo);
		} // fin while
	} // fin else
	close( fd );
}
