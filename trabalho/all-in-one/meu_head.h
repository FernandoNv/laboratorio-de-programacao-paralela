#include <math.h>

#define TRUE 1
#define FALSE 0

// --------------------------------------------------
// Tipos de envio 
#define SEND  0 // bloqueante
#define ISEND 1 // não bloqueante
#define SSEND 2 // sincrona
#define RSEND 3 // ready

// --------------------------------------------------
// Tipos de recebimento 
#define RECV  0 // bloqueante
#define IRECV 1 // não bloqueante

#define N 200000  // Numeros a serem confiridos se primos  
#define ROOT 0     // Processo central
#define REPEAT 500 // iterações do benchmarks
#define BAG_SIZE 500

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
    for( int i = 1 ; i < N ; i += 2 )
        if( primo_bf( i ) )
            count++;
    return count;

}


//TODO fazer para desvio padrao
void update_metrics( int i , double dt , double *desv_pdr , double *media ){

   if( !i ){
      *media = dt;
      *desv_pdr = 0;
      return;
    }

   // Atualizando media 
   double old_media = *media;
   *media = ( old_media*i + dt )/( i + 1 ); 

   double old_desv_padr = *desv_pdr;
   double omega_1 = i*( pow( old_desv_padr , 2 ) + pow( *media - old_media , 2 ) );
   double omega_2 = pow( *media - dt , 2 );
   *desv_pdr = sqrt( ( omega_1 + omega_2 )/( i + 1 ) );
}