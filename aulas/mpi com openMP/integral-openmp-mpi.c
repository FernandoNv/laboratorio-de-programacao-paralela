#include <stdio.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>

//TODO: Modificar
// Cada thread envia para o processo 0 seu resultado parcial

int main(int argc, char** argv) {
  double begin = clock();
  int my_rank;
  int p;                     // número de processos
  float a=0.0, b=1.0;        //intervalo a calcular
  int n=1024;                // número de trapezóides
  float  h;                 // base do trapezóide
  float local_a, local_b;   // intervalo local
  int local_n;              // número de trapezóides local
  float integral;           // integral no meu intervalo
  float total;              // integral total
  int source;               // remetente da integral
  int dest = 0;               // destino das integrais (nó 0)
  int tag = 200;              // tipo de mensagem (único)
  int resto = 0;
  MPI_Status status;
  int provided; 
  float calcula(float local_a, float local_b, int local_n, float h);

  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  h = (b-a) / n;
  local_n = n / p;
  local_a = a + my_rank * local_n * h;
  local_b = local_a + local_n * h;
  // somando o resto no final
  if(my_rank == p-1){
    local_n = (n/p) + (n%p);
    local_b = b;
  }

  //balanceamento
  resto = n%p;
  if(resto != 0){
    if(my_rank < resto) local_n = local_n + 1;
    local_a = a + my_rank * local_n * h;
    local_b = local_a + local_n * h;
  }
  integral = calcula(local_a, local_b, local_n, h);

  if(my_rank == 0){
    printf("Resultado: %f\n", integral);
    double end = clock();
    printf("Tempo: %.5f \n", (end-begin)/CLOCKS_PER_SEC);
  }
    
  MPI_Finalize();

  return 0;
}

float calcula(float local_a, float local_b, int local_n, float h) {
  float integral;
  float x;
  float f(float x); // função a integrar
  integral = (f(local_a) + f(local_b)) /2.0;
  int rank;
  int numProcesso;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcesso);
  x = local_a;

  omp_set_num_threads(4);
  #pragma omp parallel
  {
    int idThread = omp_get_thread_num();
    MPI_Status status;
    #pragma omp for reduction(+:integral)
    for(int i=1; i<=local_n; i++) {
      // x += h;
      integral += f(x + i*h);
    }
    integral *= h;

    if(rank == 0 && idThread == 0){
      float integralTemp;
      float sum = integral;
      int numThreads = omp_get_num_threads();
      //receber o valor das integrais parciais
      for(int i = 1; i < numThreads*numProcesso; i++){
        MPI_Recv(&integralTemp, 1, MPI_FLOAT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
        printf("recev parcial: %f\n", integralTemp);
        sum += integralTemp;
      }

      integral = sum;
    }else{
      //enviar o valor parcial da minha parte
      float integralTemp = integral;
      int idThread = omp_get_thread_num();
      printf("thread %i - send parcial: %f\n", idThread, integralTemp);
      MPI_Send(&integralTemp, 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
    }  
  }

  return integral;
}

float f(float x) {
  float fx; // valor de retorno
  // esta é a função a integrar
  // exemplo: função quadrática
  fx = x * x;

  return fx;
}