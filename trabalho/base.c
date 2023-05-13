#include <math.h>

int primo_bf(long int n){ /* mpi_primos.c  */

    /*

    Abordagem de força bruta para decidir se um numero
    é Primo

    */
    if( n < 2)
        return 0;
    
    for (int i = 3; i < (int)(sqrt(n) + 1); i += 2){
        if ( !( n % i ) )
            return 0;
    }

    return 1;
}

int primos_entre( long int inicio , long int fim , long int intervalo ){

    
    if( !( inicio%2) ) inicio++;
    if( !( fim%2 ) ) fim--;

    int count = 0;
    for( int i = inicio ; i < fim ; i = i + intervalo )
        count = count + primo_bf( i );
    return count;
}
