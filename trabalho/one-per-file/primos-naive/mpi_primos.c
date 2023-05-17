#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

// original: https://github.com/gpsilva2003/MPI/blob/main/src/mpi_primos.c

#define N 100000

/* mpi_primos.c  */
int primo_bf(long int n){
  /*
  Abordagem de força bruta para decidir se um numero
  é Primo
  */
  if(n < 2) return 0;

  for(int i = 3; i < (int)(sqrt(n) + 1); i += 2){
    if(!(n % i))
      return 0;
  }

  return 1;
}

int main(int argc, char *argv[]){
  double t_inicial, t_final;
  int cont = 0, total = 0;
  long int i, n;
  int meu_ranque, num_procs, inicio, salto;

  if(argc < 2){
    n = N;
  }else{
    // strtol -> String to Long?
    n = strtol(argv[1], (char **)NULL, 10);
  }

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  t_inicial = MPI_Wtime();

  /*
  num_process = 4
  
  rank | Nums
  0    |  3 , 11 , 19 , 27 , 35 .....
  1    |  5 , 13 , 21 , 29 , 37 .....
  2    |  7 , 15 , 23 , 31 , 39 .....
  3    |  9 , 17 , 25 , 33 , 41 .....
  */
  inicio = 3 + meu_ranque * 2;
  salto = num_procs * 2;
  for (i = inicio; i <= n; i += salto)
    if (primo_bf(i))
      cont++;

  if (num_procs > 1)
    MPI_Reduce(&cont, &total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  else
    total = cont;

  t_final = MPI_Wtime();
  float dt = 1000 * (t_final - t_inicial);

  if (meu_ranque == 0){
    total += 1; /* Acrescenta o dois, que também é primo */
    printf("Quant. de primos entre 1 e %ld: %d \n", n, total);
    printf("Tempo de execucao: %1.8f milisegundos \n", dt);
  }

  MPI_Finalize();
  return (0);
}