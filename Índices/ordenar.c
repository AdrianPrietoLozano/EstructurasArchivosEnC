/*

Nombre:   Prieto Lozano Adrian
Código:   217761668
Materia:  Estructuras de datos II
Sección:  D09
Profesor: Guerrero Segura Ramírez, Miguel Ángel

*/

typedef char NUMPARTI[ 5 ];
void ordenar( int indices[], const NUMPARTI numParti[], int tamanio )
{
	int aux;
	int pos;
	int x;
	NUMPARTI auxCad;
	NUMPARTI auxCad2;

	for( int i = 1; i < tamanio; i++ )
	{
		aux = indices[ i ];
		pos = i;

		for( int j = i - 1; j >= 0; j-- )
		{
			strcpy( auxCad, numParti[ aux ] );
			strcpy( auxCad2, numParti[ indices[ j ] ] );
			if( atoi( auxCad ) < atoi( auxCad2 ) )
				pos = j;
		}

		x = i;
		for( int k = i - 1; k >= pos; k-- )
		{
			indices[ x ] = indices[ k ];
			x--;
		}
		indices[ pos ] = aux;
	}
}