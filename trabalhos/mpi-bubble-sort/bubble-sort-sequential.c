#include <stdio.h>
// #include "mpi.h"

void compareExchange(double* a, double* b){
  // printf("%.2f,  %.2f\n", (*a), (*b));
  if((*a) > (*b)){
    double temp = (*a);
    (*a) = (*b);
    (*b) = temp;
  }
}

void printA(double A[], int n){
  for(int i = 0; i < n; i++){
    printf("%.2f ", A[i]);
  }
  printf("\n");
}

void bubbleSort(double A[], int n){
  for(int i = 0; i < n-1; i++){
    for (int j = 0; j < n-1; j++){
      compareExchange(&A[j], &A[j+1]);
    }
  }
}

//não esta ordenando corretamente
void OddEvenSort1(double A[], int n){
  for(int i=1; i<n; i++){
    //odd interation
    if(i%2 == 1){
      for(int j=0; j < (n/2)-2; j++)
        compareExchange(&A[2*j + 1], &A[2*j + 2]);
      // the comparison of the last pair, if n is odd
      if(n%2 == 1)
        compareExchange(&A[n-2], &A[n-1]);
    }

    // even iteration
    if(i%2 == 0){
      for(int j=1; j < (n/2) - 1; j++)
        compareExchange(&A[2*j], &A[2*j + 1]);
    }
  }
}

//https://en.wikipedia.org/wiki/Odd%E2%80%93even_sort
void OddEvenSort(double A[], int n){
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

int main(int argc, char** argv){
  double A[8] = {13,9,4,5,3,3,78,20};
  printA(A, 8);
  OddEvenSort(A, 8);
  printA(A, 8);
  return 0;
}
