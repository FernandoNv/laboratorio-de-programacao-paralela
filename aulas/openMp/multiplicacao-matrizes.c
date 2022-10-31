#include <omp.h> 
#include <stdlib.h> 
#include <stdio.h>
#define SIZE 10 /* tam matrizes */

int A[SIZE][SIZE], B[SIZE][SIZE];

int A[10][10] = {
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
};

int B[10][10] = {
  {10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
  {10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
  {10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
  {10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
  {10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
  {10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
  {10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
  {10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
  {10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
  {10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
};

int C[SIZE][SIZE];
void printM(int matrix[SIZE][SIZE]);
int main(int argc, char *argv[]){
  int i, j, k, N;
  N = SIZE;
  // fill_matrix(A);
  // fill_matrix(B);
  #pragma omp parallel for private(i,j,k) shared(A,B,C,N)
  for (i = 0; i < N; i++){
    for (j = 0; j < N; j++){
      C[i][j] = 0.0;
      for (k = 0; k < N; k++){
        C[i][j] = C[i][j] + A[i][k] * B[k][j];
      }
    }
  }
  printM(C);
}

void printM(int matrix[SIZE][SIZE]){
  for(int i = 0; i < SIZE; i++){
    for(int j = 0; j < SIZE; j++){
      printf("%d ", matrix[i][j]);
    }
    printf("\n");
  }
}

void fill_matrix(int** matrix){
  for(int i = 0; i < SIZE; i++){
    for(int j = 0; j < SIZE; j++){
      matrix[i][j] = 10;
    }
  }
}