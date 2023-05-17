/* -----------------------------------------------------------
 Esse arquivo é o benchmark para a contagem de todos os primos
 existentes entre 0 e 200000. Nele são testados todos os tipos
 de comunicação ponto a ponto e é calculado o tempo médio de 
 execução e o seu desvio padrão.
 
 O modelo de calculo de primo é por gags.

 Primeiro é medido o tempo para a contagem sem paralelismo, depois
 é feito o calculo para cada tipo de comunicação ponto a ponto a 
 ponto.
 
 Tem-se 3 tipos de envio e 2 de recebimento, totalizando seis tipos
 de comunicação.

COMPLETO
---------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"
#include "meu_head.h"

/*
Manda o processo destino a começar a contar os primos entre inicio e inicio + BAG_SIZE

A flag stop sinaliza se destino pode ser dispensado

A flag send_op indica o tipo de envio 
*/
void send_count_order( int send_op , int inicio, int destino , int stop ){
    
    // printf( "sending order starting at %d to %d\n" , inicio , destino );
    switch( send_op ){

        case ISEND:{
            MPI_Request request;
            MPI_Isend( &inicio , 1 , MPI_INT, destino , stop , MPI_COMM_WORLD , &request );
            break;
        }

        case SSEND:{
            MPI_Ssend( &inicio , 1 , MPI_INT, destino , stop , MPI_COMM_WORLD );
            break;
        }

        case RSEND:{
            MPI_Rsend( &inicio , 1 , MPI_INT, destino , stop , MPI_COMM_WORLD );
            break;
        }
    }
}

/*
    Inicializa as contagens dos processos perifericos
*/
void init_count_order( int send_op , int num_procs , int * inicio ){

    /* BAG_SIZE = 50
       3 processos perifericos
       1 - 0:50
       2 - 50:100
       3 - 100:150
    */
     
    int proc_rank = 1;
    while( proc_rank < num_procs ){
        send_count_order( send_op , *inicio, proc_rank , FALSE );
        proc_rank++;
        *inicio += BAG_SIZE;
    }

}

/*
    Retorna para root a contagem local
*/
void send_count_part( int send_op , int count ){

    switch( send_op ){

        case ISEND:{
            MPI_Request request;
            MPI_Isend( &count , 1 , MPI_INT, ROOT , 0 , MPI_COMM_WORLD , &request );
            break;
        }

        case SSEND:{
            MPI_Ssend( &count , 1 , MPI_INT, ROOT , 0 , MPI_COMM_WORLD );
            break;
        }

        case RSEND:{
            MPI_Rsend( &count , 1 , MPI_INT, ROOT , 0 , MPI_COMM_WORLD );
            break;
        }
    }
}

void recv_count_order( int recv_op , int * inicio , MPI_Status * estado ){
    if( recv_op == RECV )
        MPI_Recv( inicio , 1 , MPI_INT , ROOT , MPI_ANY_TAG , MPI_COMM_WORLD , estado );
    else{
        MPI_Request request;
        MPI_Irecv( inicio, 1, MPI_INT, ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &request );
        MPI_Wait( &request , estado );
    }
}

int recv_partial_results( int inicio , int num_procs , int send_op, int recv_op ){

    int tag, cont, total = 0, stops = 0;
    MPI_Status estado;
    MPI_Request request;
    while ( stops < num_procs - 1 ){

        if( recv_op == RECV )
            MPI_Recv(&cont, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &estado);
        else{
            MPI_Irecv( &cont, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request );
            MPI_Wait( &request , &estado );
        }

        total += cont;
        int dest = estado.MPI_SOURCE;

        tag = FALSE;
        if (inicio > N){
            tag = TRUE;
            stops++;
      }
      /* Envia um novo pedaço com TAMANHO números para o mesmo processo*/
      send_count_order( send_op , inicio , dest , tag);
      inicio += BAG_SIZE;
    }
    
    return total;
}

int root_call( int num_procs , int send_op , int recv_op ){

    int inicio = 0;
    init_count_order( send_op , num_procs, &inicio );

    int count;
    count = recv_partial_results( inicio , num_procs , send_op , recv_op );
}

void edge_call( int meu_ranque , int send_op , int recv_op ){

    int stop = FALSE;
    int inicio;
    MPI_Status estado;
    while ( !stop ){
      recv_count_order( recv_op , &inicio , &estado);
      stop = estado.MPI_TAG;
      if (!stop ){
        int count = count_prime_between( inicio , inicio + BAG_SIZE );
        send_count_part( send_op , count );
      }
    }
}

int main( int argc , char *argv[] ){

    MPI_Init(&argc, &argv);

    int meu_ranque, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    double desv_pdr , media;
    for( int send_op = 1 ; send_op < 4 ; send_op++ )
        for( int recv_op = 0 ; recv_op < 2 ; recv_op++ )
            for( int i = 0 ; i < REPEAT ; i++ ){

                if( meu_ranque == ROOT ){

                    double t_inicial = MPI_Wtime();
                    root_call( num_procs , send_op , recv_op );
                    double t_final = MPI_Wtime();

                    double dt = 1000*( t_final - t_inicial );
                    // printf( "time at iter %d = %lf\n" , i , dt);
                    update_metrics( i , dt , &desv_pdr , &media );

                    if( i == REPEAT - 1 ){
                        char *snames[ 4 ] = {"SEND","ISEND","SSEND","RSEND"};
                        char *rnames[ 2 ] = {"RECV","IRECV"};
                        printf( "\n%s , %s :", snames[ send_op ] , rnames[ recv_op ] );
                        printf( "media = %1.8f , desv_pdr = %1.8f", media , desv_pdr);
                    }
                }else{
                    edge_call( meu_ranque , send_op , recv_op );
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }

    MPI_Finalize();
    return 0;
}