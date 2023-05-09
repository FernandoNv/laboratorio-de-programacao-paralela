#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

void mxv(int n, int mult, double *A, double *b, double *c);

int main(int argc, char *argv[]){ /* mpi_mxv.c  */
  double *A, *Aloc, *b, *cloc, *c;
  /* matriz m x n por um vetor de dimensão n */
  int i, j, m, n;
  int meu_ranque, num_procs, raiz = 0;
  double inicio, fim;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  if (meu_ranque == 0){
    printf("Por favor entre com n: \n");
    scanf("%d", &n);
    printf("\n");
  }

  // um único processo(raiz) envia os mesmos dados para cada processo
  MPI_Bcast(&n, 1, MPI_INT, raiz, MPI_COMM_WORLD);

  m = num_procs*n;

  if (meu_ranque == 0){
    printf("Valor de m: %d e  n: %d \n", m, n);
    A = (double *)malloc(m * n * sizeof(double));
    c = (double *)malloc(m * sizeof(double));
  }
  Aloc = (double *)malloc(n * num_procs * sizeof(double));
  b = (double *)malloc(n * sizeof(double));
  cloc = (double *)malloc((m/num_procs) * sizeof(double));

  if (meu_ranque == 0){
    printf("Atribuindo valor inicial à matriz A e ao vetor b\n");
    for (j = 0; j < n; j++){
      b[j] = 2.0;
      printf("b[%i] = %f\n", j, b[j]);
    }
    printf("\n");
    for (i = 0; i < m; i++){
      printf("[ ");
      for (j = 0; j < n; j++){
        A[i * n + j] = (double)i;
        printf("%f ", A[i * n + j]);
      }
      printf("] \n");
    }
  }
  inicio = MPI_Wtime();
  /* Difunde o vetor b para todos os processos */
  MPI_Bcast(&b[0], n, MPI_DOUBLE, raiz, MPI_COMM_WORLD);

  /* Distribui as linhas da matriz A entre todos os processos */
  // O processo com o ranque igual a raiz distribui o conteúdo de vet_envia entre os processos.
  // O conteúdo de vet_envia é dividido em p segmentos, cada um deles consistindo de cont_envia itens.
  MPI_Scatter(A, n*(m/num_procs), MPI_DOUBLE, Aloc, n*(m/num_procs), MPI_DOUBLE, raiz, MPI_COMM_WORLD); 

  (void)mxv(n, m/num_procs, Aloc, b, cloc);

  MPI_Gather(cloc, m/num_procs, MPI_DOUBLE, c, m/num_procs, MPI_DOUBLE, raiz, MPI_COMM_WORLD);
  fim = MPI_Wtime();

  if (meu_ranque == 0){
    printf("Tempo total = %e\n", fim - inicio);
    for (i = 0; i < m; i++)
      printf("c[%d] = %3.1f ", i, c[i]);
    printf("\n");
    free(c);
    free(A);
  }
  free(Aloc);
  free(b);
  free(cloc);

  MPI_Finalize();
  return (0);
}

void mxv(int n, int mult, double *A, double *b, double *c){
  for(int i = 0; i < mult; i++){
    c[i] = 0.0;
    for (int j = 0; j < n; j++){
      c[i] += A[i * n + j] * b[j];
    }
  }
}