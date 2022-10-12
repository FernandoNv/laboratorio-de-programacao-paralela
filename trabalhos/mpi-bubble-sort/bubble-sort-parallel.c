#include <stdio.h>
#include <stdlib.h>
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

void printA(int A[], int n){
  for(int i = 0; i < n; i++){
    printf("%d ", A[i]);
  }
  printf("\n");
}

void printInner(int A[], int n, int id){
  for(int i = 0; i < n; i++){
    printf("Processo: %d - elemento: %d\n", id, A[i]);
  }
  printf("\n");
}

void compareSplit(int idS, int A[], int n){
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
  int nL = n/np, start = id*nL, end = (id+1)*nL, startS, endS;
  int* arr = (int* )malloc(nL * sizeof(int));
  int* arrS = (int* )malloc(nL * sizeof(int));
  int* result = (int* )malloc(2*nL* sizeof(int));
  MPI_Status status;

  //1.
  // printf("[%d, %d)\n", id*nL, (id+1)*nL);
  for(int i = start, j = 0; i < end; i++, j++){
    arr[j] = A[i];
  }
  printf("p(%d) - rodei a ordenação interna\n", id);
  oddEvenSort(arr, nL);
  printf("p(%d) - terminei a ordenação interna\n", id);

  if(id == idS){
    // printf("id = %d e idS = %d iguais", id, idS);
    for(int i = 0; i<n; i++) A[i] = arr[i];
    return;
  }

  //2.
  MPI_Send(&start, 1, MPI_INT, idS, 0, MPI_COMM_WORLD); //tag 0 - envio do start do arry do prcesso corrente
  MPI_Send(&end, 1, MPI_INT, idS, 1, MPI_COMM_WORLD); // tag 1 - envio do end do arry do processo corrent
  printf("p(%d) - enviei a parte do processo irmao p(%d) arr de %d elementos \n", id, idS, nL);
  
  MPI_Recv(&startS, 1, MPI_INT, idS, 0, MPI_COMM_WORLD, &status);
  MPI_Recv(&endS, 1, MPI_INT, idS, 1, MPI_COMM_WORLD, &status);
  printf("p(%d) - recebi a parte do processo irmao p(%d) arr de %d elementos \n", id, idS, nL);

  for(int i = startS, j = 0; i < endS; i++, j++){
    arrS[j] = A[i];
  }

  printf("p(%d) - Array ", id);
  printA(arr, nL);
  printf("p(%d) - ArrayS ", idS);
  printA(arrS, nL);

  //3.
  printf("p(%d) - Vou fazer o merge...\n", id);
  merge(nL, arr, arrS, result);
  printf("p(%d) - Terminei o merge...\n", id);
  printf("p(%d) - Esta ordenado? %d\n", id, isSorted(result, 2*nL));
  free(arr);
  free(arrS);

  printf("p(%d) - Array do result \n", id);
  printA(result, 2*nL);
  printf("\n");

  //4.
  if(id > idS){
    //guardar os dados da segunda metade
    for(int i = start, j = nL; i < end; i++, j++){
      A[i] = result[j];
    }
  }else{
    //guardar os dados da primeira metade
    for(int i = startS, j = 0; i < endS; i++, j++){
      A[i] = result[j];
    }
  }
  printf("p(%d) - A = ", id);
  printA(A, n);
  free(result);
  printf("p(%d) - terminei o sort\n", id);
}

void parallelOddEvenSort(int A[], int n){
  int id, np;
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  for(int i = 0; i<np; i++){
    //odd iteration
    if(i%2 == 1){
      // printf("iteracao: %d processo: %d ODD...\n", i, id);
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
      // printf("iteracao: %d processo: %d EVEN...\n", i, id);
      // Even process number
      if( id%2 == 0 ) {
        // Compare-exchange with the right neighbor process 
        if( id < np-1){
          // compareSplitMin(id+1); 
          compareSplit(id+1,A,n);
        }else{
          // um unico processo
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

void writeResult(int A[], int n, char fileName[100]){
  FILE* filePointer = fopen(fileName, "w");
  if( filePointer == NULL ){
    printf("%s file failed to open.", fileName);
  }else{  
    printf("The file is now opened.\n") ;
    
    for(int i = 0; i < n; i++){
      fprintf(filePointer,"%d", A[i]);
      fputs("\n", filePointer);
    }
    // Closing the file using fclose()
    fclose(filePointer);
    printf("Data successfully written in file %s\n", fileName);
    printf("The file is now closed.\n") ;
  }
}

void getNumbers(int* A, int n, char fileName[100]){
  FILE *filePointer = fopen(fileName, "r");
  // display numbers
  for(int i = 0; i < n; i++) fscanf(filePointer, "%d", &A[i]);
  printf("\nEnd of file.\n");
  // close connection
  fclose(filePointer);
}

int main(int argc, char** argv){
  MPI_Init(&argc, &argv);
  int id, np, count = 10, *A = (int *)malloc(sizeof(int)*count);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  if(id == 0){
    getNumbers(A, count, "4.txt");
    MPI_Bcast(A, count, MPI_INT, 0, MPI_COMM_WORLD);
  }else{
    MPI_Bcast(A, count, MPI_INT, 0, MPI_COMM_WORLD);
  }
  //printA(A, count);
  parallelOddEvenSort(A, count);
  MPI_Finalize();
  if(id == 0){
    writeResult(A, count, "result.tx");
    printf("%d\n", isSorted(A, count));
  }
  free(A);
  return 0;
}