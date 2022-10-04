#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char** argv){
  int meu_rank, np, origem, destino, tag=0;
  double inicio, fim;
  char msg[100];
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &meu_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&np);
  if(meu_rank == 0) inicio = MPI_Wtime();

  if (meu_rank != 0) {
    sprintf(msg, "Processo %d está vivo!", meu_rank);
    destino = 0;
    int tam = strlen(msg)+1;
    MPI_Send(msg, tam, MPI_CHAR, destino, tag, MPI_COMM_WORLD);
  }else {// if (meu_rank == 0)
    for (origem=1; origem<np; origem++){
      //MPI_Recv(msg, 100, MPI_CHAR, origem, tag, MPI_COMM_WORLD, &status); // imprime em ordem
      MPI_Recv(msg, 100, MPI_CHAR, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status); // imprime na ordem q a msg for recebida
      printf("%s\n", msg);
    }
    //MPI_Finalize();
    fim = MPI_Wtime();
    printf("%fs\n", (fim-inicio));
  }

  MPI_Finalize();
  return 0;
}