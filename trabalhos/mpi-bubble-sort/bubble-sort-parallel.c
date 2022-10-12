#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void compareExchange(int* a, int* b){
  // printf("%d,  %d\n", (*a), (*b));
  if((*a) > (*b)){
    int temp = (*a);
    (*a) = (*b);
    (*b) = temp;
  }
}

void oddEvenSort(int* A, int n){
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

void bubbleSort(int* A, int n){
  for(int i = 0; i < n-1; i++){
    for (int j = 0; j < n-1; j++){
      compareExchange(&A[j], &A[j+1]);
    }
  }
}

int isSorted(int* A, int n){
  for(int i = 0; i < n-1; i+=2) if(A[i] > A[i+1]) return 0;
  return 1;
}

void merge(int n, int* a, int* b, int* result){
  int i = 0, j = 0, k = 0;

  while(i < n && j < n){
    if(a[i] < b[j]) 
      result[k++] = a[i++];
    else
      result[k++] = b[j++];
  }

  while(i < n) result[k++] = a[i++];
  while(j < n) result[k++] = b[j++];
}

void printA(int A[], int start, int end){
  for(int i = start; i < end; i++){
    printf("%d ", A[i]);
  }
  printf("\n");
}

void compareSplit(int idS, int* A, int n){
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
  int nL = n/np, start = id*nL, end = (id+1)*nL;
  int* arr = (int* )malloc(nL * sizeof(int));
  int* arrS = (int* )malloc(nL * sizeof(int));
  int* result = (int* )malloc(2*nL* sizeof(int));
  MPI_Status status;

  //1.
  for(int i = start, j = 0; i < end; i++, j++) arr[j] = A[i];
  printf("p(%d) - rodei a ordenação interna\n", id);
  oddEvenSort(arr, nL);
  printf("p(%d) - terminei a ordenação interna\n", id);

  if(id == idS){
    // printf("id = %d e idS = %d iguais", id, idS);
    for(int i = 0; i<n; i++) A[i] = arr[i];
    return;
  }

  //2.
  printf("p(%d) - vou enviar a parte do processo irmao p(%d) arr de %d elementos \n", id, idS, nL);
  printf("p(%d) - vou receber a parte do processo irmao p(%d) arr de %d elementos \n", id, idS, nL);
  //Estava causando deadlock quando enviava o vetor pro processo irmao e esperava ele enviar a outra parte do vetor
  //https://stackoverflow.com/questions/15833947/mpi-hangs-on-mpi-send-for-large-messages
  MPI_Sendrecv(
    arr, nL, MPI_INT, idS, 0,
    arrS, nL, MPI_INT, idS, 0, MPI_COMM_WORLD, &status);
  // MPI_Send(arr, nL, MPI_INT, idS, 0, MPI_COMM_WORLD); //tag 0 - envio do start do arry do prcesso corrente
  printf("p(%d) - enviei a parte do processo irmao p(%d) arr de %d elementos \n", id, idS, nL);
  printf("p(%d) - recebi a parte do processo irmao p(%d) arr de %d elementos \n", id, idS, nL);
  
  // MPI_Recv(arrS, nL, MPI_INT, idS, 0, MPI_COMM_WORLD, &status);

  // for(int i = startS, j = 0; i < endS; i++, j++){
  //   arrS[j] = A[i];
  // }

  // printf("p(%d) - Array ", id);
  // printA(arr, 0, nL);
  // printf("p(%d) - ArrayS ", idS);
  // printA(arrS, 0, nL);

  //3.
  printf("p(%d) - Vou fazer o merge...\n", id);
  merge(nL, arr, arrS, result);
  printf("p(%d) - Terminei o merge...\n", id);
  // printf("p(%d) - Esta ordenado? %d\n", id, isSorted(result, 2*nL));

  // printf("p(%d) - Array do result \n", id);
  // printA(result, 0, nL*2);

  //4.
  if(id > idS){
    //guardar os dados da segunda metade
    for(int i = idS*nL, j = 0; i < end; i++, j++){
      A[i] = result[j];
    }

    // printf("p(%d) - A = ", id);
    // printA(A, idS*nL, end);
  }else{
    //guardar os dados da primeira metade
    for(int i = start, j = 0; i < (idS)*2*nL; i++, j++){
      A[i] = result[j];
    }

    // printf("p(%d) - A = ", id);
    // printA(A, start, (idS)*2*nL);
  }

  free(arr);
  free(arrS);
  free(result);
}

void parallelOddEvenSort(int A[], int n){
  int id, np;
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  for(int i = 0; i<np; i++){
    //odd iteration
    if(i%2 == 1){
      printf("p(%d) - iteração impar - ", id);
      // printf("iteracao: %d processo: %d ODD...\n", i, id);
      //odd process number
      if(id%2 == 1){
        printf("p(%d) - processo impar \n", id);
        //compare-exchange with the right neighbour process
        if(id < np-1){
          // compare_split_min(id-1); 
          compareSplit(id+1,A,n);
        }
      }else{
        printf("p(%d) - processo par \n", id);
        //compare-exchange with the left neighbour process
        if(id > 0){
          // compareSplitMax(id-1); 
          compareSplit(id-1,A,n);
        }
      }
    }

    // Even iteration
    if (i%2 == 0) {
      // printf("iteracao: %d processo: %d EVEN...\n", i, id);
      // Even process number
      printf("p(%d) - iteração par - ", id);
      if( id%2 == 0 ) {
        printf("p(%d) - processo par \n", id);
        // Compare-exchange with the right neighbor process 
        if( id < np-1){
          // compareSplitMin(id+1); 
          compareSplit(id+1,A,n);
        }else{
          // um unico processo
          compareSplit(id,A,n);
        }
      }else{
        printf("p(%d) - processo impar \n", id);
        // Compare-exchange with the left neighbor process
        // compareSplitMax(id-1); 
        compareSplit(id-1,A,n);
      }
    }
  }
}

void writeResult(int* A, int n, char fileName[100]){
  FILE* filePointer = fopen(fileName, "w");
  if( filePointer == NULL ){
    printf("%s falha ao abrir o arquivo", fileName);
  }else{  
    printf("Arquivo aberto.\n") ;
    
    for(int i = 0; i < n; i++){
      fprintf(filePointer,"%d", A[i]);
      fputs("\n", filePointer);
    }
    // Closing the file using fclose()
    fclose(filePointer);
    printf("Dados salvos %s\n", fileName);
    printf("Arquivo fechado.\n");
  }
}

void getNumbers(int* A, int n, char fileName[100]){
  FILE *filePointer = fopen(fileName, "r");
  // display numbers
  for(int i = 0; i < n; i++) fscanf(filePointer, "%d", &A[i]);
  printf("\nFim do arquivo.\n");
  // close connection
  fclose(filePointer);
}

int main(int argc, char** argv){
  int id, np, count = 40000, *A = (int *)malloc(sizeof(int)*count);
  double stopT, startT;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  if(id == 0){
    getNumbers(A, count, "4.txt");
  }
  MPI_Bcast(A, count, MPI_INT, 0, MPI_COMM_WORLD);
  startT = clock();
  parallelOddEvenSort(A, count);
  if(id == 0){
    double stopT = clock();
    printf("%.5f s\n", (stopT-startT)/CLOCKS_PER_SEC);
  }
  //printf("%d\n", isSorted(A, count));
  MPI_Finalize();
  // if(id == 0 && isSorted(A, count)){
  //   writeResult(A, count, "result.tx");
  // }
  free(A);
  return 0;
}