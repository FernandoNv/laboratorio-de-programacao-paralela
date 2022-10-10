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

void oddEvenSort(int A[], int n){
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

void merge(int n, int a[n], int b[n], int result[2*n]){
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
  int nL = n/np;
  int arr[nL];
  int arrS[nL];
  int result[2*nL];
  MPI_Status status;

  //1.
  // printf("[%d, %d)\n", id*nL, (id+1)*nL);
  for(int i = id*nL, j = 0; i < (id+1)*nL; i++, j++){
    arr[j] = A[i];
  }
  
  compareSplit(idS, A, n/np);

  if(n < np){
    //ordenação basica
    
  }

  if(id == idS){
    // printf("id = %d e idS = %d iguais", id, idS);
    for(int i = 0; i<n; i++) A[i] = arr[i];
    return;
  }

  //2.
  MPI_Send(arr, nL, MPI_INT, idS, 0, MPI_COMM_WORLD);
  MPI_Recv(arrS, nL, MPI_INT, idS, 0, MPI_COMM_WORLD, &status);

  //3.
  merge(nL, arr, arrS, result);

  // printf("Array do id ");
  // printA(arr, nL);
  // printf("Array do idS ");
  // printA(arrS, nL);
  // printf("Array do result ");
  // printA(result, 2*nL);
  // printf("\n");

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
    printf("The file is now closed.") ;
  }
}

void getNumbers(int A[], int n, char fileName[100]){
  FILE *filePointer = fopen("1.txt", "r");
  // display numbers
  // printf("\nNumbers:\n");
  for(int i = 0; i < n; i++) fscanf(filePointer, "%d", &A[i]);
  printf("\nEnd of file.\n");
  // close connection
  fclose(filePointer);
}

int main(int argc, char** argv){
  MPI_Init(&argc, &argv);
  int id, np, count = 10000, A[count];
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Status status;

  if(id == 0){
    getNumbers(A, count, "1.txt");
    for(int i = 1; i < np; i++){
      MPI_Send(A, count, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
  }else{
    MPI_Recv(A, count, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
  }
  //printA(A, count);
  parallelOddEvenSort(A, count);
  MPI_Finalize();
  if(id == 0) writeResult(A, count, "result.tx");
  return 0;
}