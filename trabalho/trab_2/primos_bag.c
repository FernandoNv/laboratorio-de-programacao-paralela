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

    #pragma omp parallel for reduction(+:total) schedule( dynamic,BAG )
        for( i = 3 ; i < N ; i += 2 )
            total += primo_bf( i );

    t_fim = omp_get_wtime();

    printf( "\n\nExiste um total de %ld primos entre 0 e %d" , total , N );
    printf( "\nA contagem demorou %.2lf ms com %d threads \n", 1000*( t_fim - t_inicio ), N_THREADS );
    return 0;
}