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
    Inicializa as contagens dos processos perifericos
*/
void init_count_order( int send_op , int num_procs , int * inicio ){

    int proc_rank = 1;
    while( proc_rank < num_procs ){
        send_count_order( send_op , proc_rank , FALSE );
        proc_rank++;
        *inicio += BAG_SIZE;
    }

}

/*
Manda o processo destino a começar a contar os primos entre inicio e 
inicio + BAG_SIZE

A flag stop sinaliza se destino pode ser dispensado

A flag send_op indica o tipo de envio 
*/
void send_count_order( int send_op , int inicio, int destino , int stop ){

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

void send_count_part( int send_op , int count );

int recv_partial_results( );

int irecv_partial_results( );
