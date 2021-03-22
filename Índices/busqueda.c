/*

Nombre:   Prieto Lozano Adrian
Código:   217761668
Materia:  Estructuras de datos II
Sección:  D09
Profesor: Guerrero Segura Ramírez, Miguel Ángel

*/

typedef char NUMPARTI[ 5 ];
int busquedaBinaria( const int indices[], const NUMPARTI numParti[], const char clave[], const int tamanio )
{
	int bajo = 0;
	int alto = tamanio - 1;
	int centro;
	NUMPARTI aux;

	while( bajo <= alto )
	{
		centro = ( int ) ( ( bajo + alto ) / 2 );
		strcpy( aux, numParti[ indices[ centro ] ] );
		if( atoi( clave ) < atoi( aux ) )
			alto = centro - 1;
		else if( atoi( clave ) > atoi( aux ) )
			bajo = centro + 1;
		else // se encontro la clave
			return indices[ centro ];
	}
	return -1;
}