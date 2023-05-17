#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"
#define TAMANHO 5000
#define N 200000
int primo(int n){
  int i;
  for (i = 3; i < (int)(sqrt(n) + 1); i += 2){
    if (n % i == 0)
      return 0;
  }

  return 1;
}

/* mpi_primosbag.c  */
int main(int argc, char *argv[]){
  double t_inicial, t_final;
  int cont = 0, total = 0;
  int i, n;
  int meu_ranque, num_procs, inicio, dest, raiz = 0, tag = 1;
  MPI_Status estado;
  /* Verifica o número de argumentos passados */
  // if (argc < 2){
  //   printf("Entre com o valor do maior inteiro como parâmetro para o programa.\n");
  //   return 0;
  // } else {
  //   n = strtol(argv[1], (char **)NULL, 10);
  // }
  n = N;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Request pedido_recebimento;
  
  /* Se houver menos que dois processos aborta */
  if (num_procs < 2){
    printf("Este programa deve ser executado com no mínimo dois processos.\n");
    MPI_Abort(MPI_COMM_WORLD, 1);

    return 1;
  }

  /* Registra o tempo inicial de execução do programa */
  t_inicial = MPI_Wtime();
  
  /* Envia pedaços com TAMANHO números para cada processo */
  if (meu_ranque == 0){

    // total = 5000
    //
    // TAM = 50
    // 1 - 0 : 50
    // 2 - 50 : 100
    // 3 - 100 : 150
    for (dest = 1, inicio = 3; dest < num_procs && inicio < n; dest++, inicio += TAMANHO){
      MPI_Ssend(&inicio, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
    }

    /* Fica recebendo as contagens parciais de cada processo */
    int stop = 0;
    while (stop < (num_procs - 1) ){

      // cont vai ser a resposta do perifeco
      int terminado_receber = 0;
      MPI_Irecv(&cont, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &pedido_recebimento);
      MPI_Wait(&pedido_recebimento, &estado);
      
      total += cont;
      dest = estado.MPI_SOURCE;
      if (inicio > n){
        tag = 99;
        stop++;
      }
      /* Envia um novo pedaço com TAMANHO números para o mesmo processo*/
      MPI_Ssend(&inicio, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
      inicio += TAMANHO;
    }
  } else {
    /* Cada processo escravo recebe o início do espaço de busca */
    while (estado.MPI_TAG != 99){
      int terminado_receber = 0;
      MPI_Irecv(&cont, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &pedido_recebimento);
      MPI_Wait(&pedido_recebimento, &estado);

      if (estado.MPI_TAG != 99){
        for (i = inicio, cont = 0; i < (inicio + TAMANHO) && i < n; i += 2)
          if (primo(i) == 1)
            cont++;
        /* Envia a contagem parcial para o processo mestre */
        MPI_Ssend(&cont, 1, MPI_INT, raiz, tag, MPI_COMM_WORLD);
      }
    }

    /* Registra o tempo final de execução */
    t_final = MPI_Wtime();
  }

  if (meu_ranque == 0){
    t_final = MPI_Wtime();
    total += 1; /* Acrescenta o 2, que é primo */
    printf("Quant. de primos entre 1 e %d: %d \n", n, total);
    printf("Tempo de execucao: %1.3f \n", 1000*( t_final - t_inicial ) );
  }

  /* Finaliza o programa */
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();

  return 0;
}