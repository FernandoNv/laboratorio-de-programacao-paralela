#include <math.h>

#define N 5000000
// #define N 100
#define N_THREADS 16
#define BAG 10000

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

int count_prime_between( int start , int end ){


    if( !( start%2 ) )
        start++;
    
    if ( !( end%2 ) )
        end--;
    
    int count = 0;
    for( int i = start ; i < end ; i += 2 )
        count += primo_bf( i );
    return count;

}