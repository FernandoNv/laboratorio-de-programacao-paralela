#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "meu_head.h"

int main(){

    long int i , tid, local, inicio;

    // total de primos, comecando com 1 para
    // ja ir considerando o número 2
    long int total = 1;
    double t_inicio , t_fim;
    
    omp_set_num_threads( N_THREADS );           // 4 threads
    t_inicio = omp_get_wtime();
    #pragma omp parallel private( local , tid , inicio, i ) shared( total )
    {

        // achando o id da thread
        tid = omp_get_thread_num();
        local = 0;

        /* Contando primos na particao local

        num_process = 4

        rank | Nums 
        0    |  3 , 11 , 19 , 27 , 35 .....
        1    |  5 , 13 , 21 , 29 , 37 .....
        2    |  7 , 15 , 23 , 31 , 39 .....
        3    |  9 , 17 , 25 , 33 , 41 ..... 

        */
        inicio = 3 + tid * 2;
        for ( i = inicio ; i < N ; i += 2*N_THREADS )
            // if( primo_bf( i ) )
            //     local += 1;

            local += primo_bf( i );
        
        #pragma omp critical( total )
        {
            // printf( "\nprimos contados pela thread %ld: %ld" , tid , local );
            total = total + local;
        }
    }
    t_fim = omp_get_wtime();

    printf( "\nExiste um total de %ld primos entre 0 e %d" , total , N );
    printf( "\nA contagem demorou %.2lf ms com %d threads \n", 1000*( t_fim - t_inicio ), N_THREADS );
    return 0;
}