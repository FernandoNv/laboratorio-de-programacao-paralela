#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void compareExchange(int* a, int* b){
  // printf("%.2f,  %.2f\n", (*a), (*b));
  if((*a) > (*b)){
    double temp = (*a);
    (*a) = (*b);
    (*b) = temp;
  }
}

void bubbleSort(int* A, int n){
  for(int i = 0; i < n-1; i++){
    for (int j = 0; j < n-1; j++){
      compareExchange(&A[j], &A[j+1]);
    }
  }
}

//não esta ordenando corretamente
// void OddEvenSort1(double A[], int n){
//   for(int i=1; i<n; i++){
//     //odd interation
//     if(i%2 == 1){
//       for(int j=0; j < (n/2)-2; j++)
//         compareExchange(&A[2*j + 1], &A[2*j + 2]);
//       // the comparison of the last pair, if n is odd
//       if(n%2 == 1)
//         compareExchange(&A[n-2], &A[n-1]);
//     }

//     // even iteration
//     if(i%2 == 0){
//       for(int j=1; j < (n/2) - 1; j++)
//         compareExchange(&A[2*j], &A[2*j + 1]);
//     }
//   }
// }

void writeResult(int* A, int n, char fileName[100]){
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
  // printf("\nNumbers:\n");
  for(int i = 0; i < n; i++) fscanf(filePointer, "%d", &A[i]);
  printf("\nEnd of file.\n");
  // close connection
  fclose(filePointer);
}

//https://en.wikipedia.org/wiki/Odd%E2%80%93even_sort
void OddEvenSort(int* A, int n){
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

int isSorted(int* A, int n){
  for(int i = 0; i < n-1; i+=2) if(A[i] > A[i+1]) return 0;
  return 1;
}

int main(int argc, char** argv){
  int count = 40000, *A = malloc(sizeof(int)*count);
  double stopT, startT;
  getNumbers(A, count, "4.txt");
  startT = clock();
  OddEvenSort(A, count);
  stopT = clock();
  printf("%.5f s\n", (stopT-startT)/CLOCKS_PER_SEC);
  // printf("%d\n", isSorted(A, count));
  // writeResult(A, count, "result.tx");
  free(A);
  return 0;
}
