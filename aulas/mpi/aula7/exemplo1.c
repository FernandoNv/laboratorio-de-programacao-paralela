#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[]){
    int n, meu_ranque, num_procs, i, rc;
    double mypi, pi, h, x, sum = 0.0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    if (meu_ranque == 0){
        printf("Entre com o número de intervalos: \n");
        scanf("%d", &n);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (n != 0){
        h = 1.0 / (double)n;
        for (i = meu_ranque + 1; i <= n; i += num_procs){
            x = h * ((double)i - 0.5);
            sum += (4.0 / (1.0 + x * x));
        }
        mypi = h * sum;
        MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        if (meu_ranque == 0)
            printf("valor aproximado de pi: %.16f \n", pi);
    }
    MPI_Finalize();
}