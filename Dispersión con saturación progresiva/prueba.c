int dispersion( const char llave[] )
{
    // llena la el sobrante de la llave con espacios
    char llaveCopia[ 64 ];
    strcpy( llaveCopia, llave );
    if( strlen( llaveCopia ) < 12 )
        for( int i = strlen( llaveCopia ); i < 12; i++ )
            llaveCopia[ i ] = ' ';
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

int main( void )
{


    printf("%6s\n", "Hola:");


    printf("%i \n", dispersion( "Esparza" ));
    printf("%i \n", dispersion( "Lopez" ));
    printf("%i \n", dispersion( "Gallardo" ));
    printf("%i \n", dispersion( "Avila" ));
    printf("%i \n", dispersion( "Villarreal" ));
    printf("%i \n", dispersion( "Prieto" ));
    printf("%i \n", dispersion( "Vera" ));
    printf("%i \n", dispersion( "Gallardo" ));
    printf("%i \n", dispersion( "Montes" ));
    printf("%i \n", dispersion( "Zamora" ));
    printf("%i \n", dispersion( "Gutierrez" ));
    printf("%i \n", dispersion( "Oca" ));
    printf("%i \n", dispersion( "Mora" ));
    printf("%i \n", dispersion( "O" ));


    return 0;
}