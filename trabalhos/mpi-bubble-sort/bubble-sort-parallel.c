#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void compareExchange(double* a, double* b){
  // printf("%.2f,  %.2f\n", (*a), (*b));
  if((*a) > (*b)){
    double temp = (*a);
    (*a) = (*b);
    (*b) = temp;
  }
}

void oddEvenSort(double A[], int n){
  int sorted = 0;
  for (int i=1; i<n; i++) {
    for (int j = 1; j < n - 1; j += 2) {
      if (A[j] > A[j + 1]) {
        compareExchange(&A[j], &A[j + 1]);
      }
    }
    for (int j = 0; j < n - 1; j += 2) {
      if (A[j] > A[j + 1]) {
        compareExchange(&A[j], &A[j + 1]);
      }
    }
  }
}

void merge(double a[], double b[], int n, double result[2*n]){
  int i = 0, j = 0, k = 0;

  while(i < n && j < n){
    if(a[i] < b[j]){
      result[k] = a[i];
      i++;
    }else{
      result[k] = b[j];
      j++;
    }
    k++;
  }

  while(i < n){
    result[k] = a[i];
    i++;
    k++;
  }

  while(j < n){
    result[k] = b[j];
    j++;
    k++;
  }
}

void printA(double A[], int n){
  for(int i = 0; i < n; i++){
    printf("%.2f ", A[i]);
  }
  printf("\n");
}

void printInner(double A[], int n, int id){
  for(int i = 0; i < n; i++){
    printf("Processo: %d - elemento: %.2f\n", id, A[i]);
  }
  printf("\n");
}

void compareSplit(int idS, double A[], int n){
  // 1. Copiar os dados para um array novo e ordena-lo com o OddEvenSort
  // 2. Enviar esse array pro processo irmao
  // 3. Juntar os dois arrays fazendo a ordenação
  // 4. Pegar a parte relacionado do processo e guardar na memoria do array final
      //  Caso id > idS
      //    ele eh o processo da direita. Pegar os elementos da segunda metade
      //  Caso id < idS
      //    ele eh o processo da esqueda. Pegar os elementos da primeira metade
  int id, np;
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  int nL = n/np;
  double arr[nL];
  double arrS[nL];
  double result[2*nL];

  //1.
  for(int i = id*nL, j = 0; i < (id+1)*nL; i++, j++){
    arr[j] = A[i];
  }
  oddEvenSort(arr, nL);
  if(id == idS){
    for(int i = 0; i<n; i++) A[i] = arr[i];
    return;
  }

  //2.
  MPI_Send(arr, nL, MPI_DOUBLE, idS, 0, MPI_COMM_WORLD);
  MPI_Recv(arrS, nL, MPI_DOUBLE, idS, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  //3.
  merge(arr, arrS, nL, result);

  printInner(arr, nL, id);
  printInner(arrS, nL, id);
  printInner(result, 2*nL, id);
  printf("\n");

  //4.
  if(id > idS){
    //guardar os dados da segunda metade
    for(int i = id*nL, j = nL; i < (id+1)*nL; i++, j++){
      A[i] = result[j];
    }
  }else{
    //guardar os dados da primeira metade
    for(int i = id*nL, j = 0; i < (idS+1)*nL; i++, j++){
      A[i] = result[j];
    }
  }
}


void parallelOddEvenSort(double A[], int n){
  int id, np;
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  for(int i = 0; i<np; i++){
    //odd iteration
    if(i%2 == 1){
      printf("iteracao: %d processo: %d ODD...\n", i, id);
      //odd process number
      if(id%2 == 1){
        //compare-exchange with the right neighbour process
        if(id < np-1){
          // compare_split_min(id-1); 
          compareSplit(id+1,A,n);
        }
      }else{
        //compare-exchange with the left neighbour process
        if(id > 0){
          // compareSplitMax(id-1); 
          compareSplit(id-1,A,n);
        }
      }
    }

    // Even iteration
    if (i%2 == 0) {
      printf("iteracao: %d processo: %d EVEN...\n", i, id);
      // Even process number
      if( id%2 == 0 ) {
        // Compare-exchange with the right neighbor process 
        if( id < np-1){
          // compareSplitMin(id+1); 
          compareSplit(id+1,A,n);
        }else{
          compareSplit(id,A,n);
        }
        
      }else{
        // Compare-exchange with the left neighbor process
        // compareSplitMax(id-1); 
        compareSplit(id-1,A,n);
      }
    }
  }
}

int main(int argc, char** argv){
  MPI_Init(&argc, &argv);
  int id, np;
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  double A[12] = {12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
  parallelOddEvenSort(A, 12);
  MPI_Finalize();
  if(id == 0) printA(A, 12);
  return 0;
}