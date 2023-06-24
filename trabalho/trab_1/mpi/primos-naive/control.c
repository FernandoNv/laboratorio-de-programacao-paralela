#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define TRUE 1
#define FALSE 0

#define SEND  0
#define ISEND 1
#define SSEND 2
#define RSEND 3

#define RECV  0
#define IRECV 1

#define N 200000 // Numeros a serem confiridos se primos  
#define ROOT 0    // Processo central

// ---------------------------------------------------------------------
// ESCOLHA DE PARÂMETROS 
void escolher_send( int *op ){

    while( TRUE ){

        printf( "opcoes de envio:\n" );

        printf("\n0 - Send");
        printf( "\n1 - ISend" );
        // printf( "\n2 - SSend" );
        // printf( "\n3 - RSend" );

        printf( "\n\nSua escolha: " );
        scanf("%d\n" , op );

        if( ( *op > 1 ) || ( *op < 0 ) )
            printf( "\nescolha inválida\n");
        else
            break;

    }

}

void escolher_recv( int *op ){

    while( TRUE ){

        printf( "\nopcoes de recebimento:\n" );

        printf("\n0 - Recv");
        printf( "\n1 - IRecv" );

        printf( "\n\nSua escolha: " );
        scanf("%d\n" , op );

        if( ( *op > 1 ) || ( *op < 0 ) )
            printf( "\nescolha inválida\n");
        else
            break;

    }

}

void escolher_reps( int *num_rep ){

    while( TRUE ){

        printf( "\nnumero de repeticoes: " );
        scanf("%d\n" , num_rep );

        if( *num_rep < 1  )
            printf( "\nescolha inválida\n");
        else
            break;

    }

}

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

int contar_primos( int meu_ranque , int num_procs ){
    
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

int main( int argc, char *argv[] ){

    // int send_op;
    // escolher_send( &send_op );

    // int recv_op;
    // escolher_recv( &recv_op );

    // int num_repeats;
    // escolher_reps( &num_repeats );

    int send_op = SSEND;
    int recv_op = RECV;
    int num_repeats = 20;

    // printf( "\n%d %d %d" , send_op , recv_op , num_repeats );

    double media, desv_pdr;
    media = 0;
    desv_pdr = -1;

    MPI_Init(&argc, &argv);

    int meu_ranque, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    for( int i = 0 ; i < num_repeats ; i++ ){

        if( meu_ranque == ROOT ){
            
            double t_inicial = MPI_Wtime();

            int cont = contar_primos( meu_ranque , num_procs );
            int total = receber_msg( num_procs , recv_op ) + cont;

            double t_final = MPI_Wtime();  
            float dt = 1000*( t_final - t_inicial );

            // TODO
            // update_metrics( i , dt , &desv_pdr , &media );

            if( !i )
                printf("\nQuant. de primos entre 1 e %d: %d \n", N , total );
            printf( "\nTempo de execucao para i = %d: %1.8f ms", i , dt );

            if( i == num_repeats - 1 )
                printf( "\n" );
        }
        else{
            int cont = contar_primos( meu_ranque , num_procs );
            enviar_msg( meu_ranque , send_op , cont );
        }

    }

    MPI_Finalize();
    return 0;

}
