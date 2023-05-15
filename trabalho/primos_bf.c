/* -----------------------------------------------------------
 Esse arquivo é o benchmark para a contagem de todos os primos
 existentes entre 0 e 200000. Nele são testados todos os tipos
 de comunicação ponto a ponto e é calculado o tempo médio de 
 execução e o seu desvio padrão.
 
 O modelo de calculo de primo é por força bruta.

 Primeiro é medido o tempo para a contagem sem paralelismo, depois
 é feito o calculo para cada tipo de comunicação ponto a ponto a 
 ponto.
 
 Tem-se 4 tipos de envio e 2 de recebimento, totalizando oito tipos
 de comunicação.

 INCOMPLETO
 ---------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"


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
#define ROOT 0    // Processo central
#define REPEAT 100 // iterações do benchmarks

//----------------------------------------------------------------------
// CALCULO DO PRIMO
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

int contar_primos_par( int meu_ranque , int num_procs ){
    
    /*

    num_process = 4

    rank | Nums 
    0    |  3 , 11 , 19 , 27 , 35 .....
    1    |  5 , 13 , 21 , 29 , 37 .....
    2    |  7 , 15 , 23 , 31 , 39 .....
    3    |  9 , 17 , 25 , 33 , 41 ..... 

    */

    int cont = 0;   
    int inicio = 3 + meu_ranque * 2;
    int salto = num_procs * 2;
    for ( int i = inicio; i <= N; i += salto)
        if ( primo_bf(i) )
            cont++;
    return cont;
}

int contar_primos_ser(){

    /* --------------------------------------
    Contagem de primos sem o uso de paralelismo
    ---------------------------------------- */

    int cont = 1; // considerando 2 que é o unico par primo
    for( int i = 3 ; i < N ; i += 2 )
        if( primo_bf( i ) )
            cont++;
    return cont;

}

// --------------------------------------------------------------------
// TROCA DE MSGS
int receber_msg( int num_procs , int recv_op ){

    int total = 1;
    switch( recv_op ){

        case RECV:

            int cont_outro; 
            for( int i = 1 ; i < num_procs ; i++ ){
                MPI_Recv( &cont_outro , 1 , MPI_INT , i , 0 , MPI_COMM_WORLD , MPI_STATUS_IGNORE );
                total = total + cont_outro;
            }
            break;
        
        case IRECV:

            MPI_Request *request;
            request = ( MPI_Request *) malloc( sizeof( MPI_Request )*( num_procs - 1 ) );

            int *cont_outros;
            cont_outros = ( int * ) malloc( sizeof( int )*( num_procs - 1 ) );

            for( int i = 1 ; i < num_procs ; i++ )
                MPI_Irecv( &cont_outros[ i - 1 ] , 1 , MPI_INT , i , 0 , MPI_COMM_WORLD , &request[ i - 1 ] );
            MPI_Waitall( num_procs - 1 , request, MPI_STATUSES_IGNORE ); 

            for( int i = 0 ; i < num_procs - 1 ; i++ ){
                total = total + cont_outros[ i ];
            }

            break;
    }

    return total;

}

void enviar_msg( int meu_ranque , int send_op , int cont ){
    
    switch( send_op ){

        case SEND:
            MPI_Send( &cont , 1 , MPI_INT, 0 , 0 , MPI_COMM_WORLD );
            break;
        
        case ISEND:
            MPI_Request request;
            MPI_Isend( &cont , 1 , MPI_INT, 0 , 0 , MPI_COMM_WORLD , &request );
            break;

        // TODO
        case SSEND:
            MPI_Ssend( &cont , 1 , MPI_INT, 0 , 0 , MPI_COMM_WORLD );
            break;
        
        // TODO
        case RSEND:
            MPI_Rsend( &cont , 1 , MPI_INT, 0 , 0 , MPI_COMM_WORLD );
            break;
    }
}

//TODO fazer para desvio padrao
void update_metrics( int i , double dt , double *desv_pdr , double *media ){

   if( !i ){
      *media = dt;
      *desv_pdr = 0;
      return;
   }

   double old_media = *media;
   *media = ( old_media*i + dt )/( i + 1 ); 
}

int par_bench( int send_op , int recv_op , int num_procs, int meu_ranque ){

    /* ----------------------------------------------------------
    Faz o benchmarking com o modelo de paralelismo escolhido
    ------------------------------------------------------------ */
    double media, desv_pdr;
    for( int i = 0 ; i < REPEAT ; i++ ){

        if( meu_ranque == ROOT ){
            
            double t_inicial = MPI_Wtime();

            int cont = contar_primos_par( meu_ranque , num_procs );
            int total = receber_msg( num_procs , recv_op ) + cont;

            double t_final = MPI_Wtime();  
            float dt = 1000*( t_final - t_inicial );
            update_metrics( i , dt , &desv_pdr , &media );
            
            if( i == REPEAT - 1 ){
                char *snames[ 4 ] = {"SEND","ISEND","SSEND","RSEND"};
                char *rnames[ 2 ] = {"RECV","IRECV"};
                printf( "\n%s , %s :", snames[ send_op ] , rnames[ recv_op ] );
                printf( "media = %1.8f , desv_pdr = %1.8f", media , desv_pdr);
            }
        }
        else{
            int cont = contar_primos_par( meu_ranque , num_procs );
            enviar_msg( meu_ranque , send_op , cont );
        }

    }
    return 0;
}

void ser_bench( int meu_ranque ){

    double media, desv_pdr;
    double t_inicial , t_final , dt;
    for( int i = 0 ; i < REPEAT ; i++ ){
        
        t_inicial = MPI_Wtime();
        contar_primos_ser();
        t_final = MPI_Wtime();

        dt = 1000*( t_final - t_inicial );
        update_metrics( i , dt , &desv_pdr , &media );

    }

    if( meu_ranque == ROOT )
        printf( "\nsem paralelismo: media = %1.8f , desv_pdr = %1.8f\n", media , desv_pdr);
}

int main( int argc , char *argv[] ){

    MPI_Init(&argc, &argv);

    int meu_ranque, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if( meu_ranque == ROOT){
        printf( "contagem de primos( tempos em milisegundos )");
        printf("\nmax_val = %d", N );
        printf("\nnum_procs = %d", num_procs );
        printf("\niteracoes = %d\n", REPEAT );
    }
    ser_bench( meu_ranque );

    for( int send_op = 0 ; send_op < 4 ; send_op++ )
        for( int recv_op = 0 ; recv_op < 2 ; recv_op++ )
            par_bench( send_op , recv_op , num_procs , meu_ranque );

    if ( meu_ranque == ROOT )
        printf( "\n");

    MPI_Finalize();

    return 0;
}