/*

Nombre:   Prieto Lozano Adrian
Código:   217761668
Materia:  Estructuras de datos II
Sección:  D09
Profesor: Guerrero Segura Ramírez, Miguel Ángel

*/

#include "arches.h"

int toma_reg( int fd, char buffreg[] )
{
	int long_reg;

	if( read( fd, &long_reg, 2 ) == 0 )
		return 0;
	long_reg = read( fd, buffreg, long_reg );

	return long_reg;
}

int toma_campo( char campo[], const char buffreg[], short pos_bus, short long_reg, int numCampo )
{
	int cpos = 0;
	char aux[ strlen( campo ) + 2 ];

	if( pos_bus == long_reg )
		return 0;

	if( numCampo == 1 )
	{
		memcpy( campo, buffreg, 4 );
		campo[ 4 ] = '\0';
		return 4;
	}
	else if( numCampo == 2 )
	{
		int x = 0;
		int y;
		for( y = 4; y < 10; y++ )
		{
			campo[ x ] = buffreg[ y ];
			x++;
		}
		campo[ y ] = '\0';

		for( int i = 0; i < strlen( campo ); aux[ i++ ] = campo[ i ] );
		for( int i = 0; i < strlen( campo ) + 3; campo[ i++ ] = '\0' );
		int j = 0;
		for( int i = 0; i < 6; i++ )
		{
			campo[ j ] = aux[ i ];
			if( i == 1 || i == 3  )
				campo[ ++j ] = ':';
			j++;
		}
		campo[ j ] = '\0';
		return 10;
	}
	else
	{
		while( pos_bus < long_reg && ( campo[ cpos++ ] = buffreg[ pos_bus++ ] ) != DELIM_CAR );
		if( campo[ cpos - 1 ] == DELIM_CAR )
			campo[ --cpos ] = '\0';
		else
			campo[ cpos ] = '\0';
		return pos_bus;
	}
}
