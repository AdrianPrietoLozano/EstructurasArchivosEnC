#include "arches.h"

int toma_reg( int fd, char buffreg[] )
{
	int long_reg;

	if( read( fd, &long_reg, 2 ) == 0 )
		return 0;
	long_reg = read( fd, buffreg, long_reg );

	return long_reg;
}

int toma_campo( char campo[], char buffreg[], short pos_bus, short long_reg, int numCampo )
{
	int cpos = 0;
	char aux[ strlen( campo ) + 2 ];

	if( pos_bus == long_reg )
		return 0;

	while( pos_bus < long_reg && ( campo[ cpos++ ] = buffreg[ pos_bus++ ] ) != DELIM_CAR );

	if( numCampo == 2 )
	{
		for( int i = 0; i < strlen( campo ); aux[ i++ ] = campo[ i ] );
		for( int i = 0; i < strlen( campo ); campo[ i++ ] = '\0' );
		int j = 0;
		for( int i = 0; i < strlen( aux ); i++ )
		{
			campo[ j ] = aux[ i ];
			if( i == 1 || i == 3  )
				campo[ ++j ] = ':';
			j++;
		}
		cpos++;
	}

	if( campo[ cpos - 1 ] == DELIM_CAR )
		campo[ --cpos ] = '\0';
	else
		campo[ cpos ] = '\0';

	return pos_bus;
}
