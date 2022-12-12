#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// Structure for the pivot row selection
typedef struct PivotRowSelection{
  double MaxValue;
  int ProcRank;
} PivotRowSelection;

// Program 9.1
// The Gauss Algorithm for solving the systems of linear equations
int ProcNum;            // Number of the available processes
int ProcRank;           // Rank of the current process
int *pParallelPivotPos; // Number of rows selected as the pivot ones
int *pProcPivotIter;    // Number of iterations, at which the processor
                        // rows were used as the pivot ones

int *pProcInd; // Number of the first row located on the processes
int *pProcNum; // Number of the linear system rows located on the processes

void ParallelBackSubstitution(double *Matrix, double *Vector, double *Result, int Size);
void ParallelEliminateColumns(double *Matrix, double *Vector, int i, int Size);
void ParallelGaussianElimination(double *Matrix, double *Vector, int Size);
void SwapRow(double* A, int i, int j, int Size);
void Swap(double* A, int i, int j, int Size);
void ProcessInitialization(double **pMatrix, double **pVector, double **pResult, double **pProcRows, double **pProcVector, double **pProcResult, int Size, int RowNum);
void ParallelResultCalculation(double *Matrix, double *Vector, double *Result, int Size);
void Initialization(double *Matrix, double *Vector, int Size);
void printMatrix(double* Matrix, int Size);
void printVector(double* Vector, int Size);

void main(int argc, char **argv){
  int Size = 8;            // Size of the matrix and vectors
  int RowNum = 8;          // Number of the matrix rows
  double *Matrix = (double*)malloc(sizeof(double) * Size * Size);     // Matrix of the linear system
  double *Vector = (double*)malloc(sizeof(double) * Size);     // Right parts of the linear system
  double *Result = (double*)malloc(sizeof(double) * Size);     // Result vector
  double *pProcRows;   // Rows of the matrix A
  double *pProcVector; // Block of the vector b
  double *pProcResult; // Block of the vector x

  setvbuf(stdout, 0, _IONBF, 0);
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
  MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);

  if (ProcRank == 0){
    printf("Parallel Gauss algorithm for solving linear systems\n");
    // Memory allocation and data initialization
    Initialization(Matrix, Vector, Size);
  }
  MPI_Bcast(Matrix, Size * Size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(Vector, Size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  
  // printf("p(%d) \n", ProcRank);
  // printMatrix(Matrix, Size);
  // printVector(Vector, Size);

  ParallelResultCalculation(Matrix, Vector, Result, Size);
  // printVector(Result, Size);

  // ResultCollection(pProcResult, Result);

  // TestResult(pMatrix, pVector, pResult, Size);

  // // Computational process termination

  MPI_Finalize();
}

void ParallelResultCalculation(double *Matrix, double *Vector, double *Result, int Size){
  // Gaussian elimination
  ParallelGaussianElimination(Matrix, Vector, Size);
  // Back substitution
  ParallelBackSubstitution(Matrix, Vector, Result, Size);
}

void Initialization(double *Matrix, double *Vector, int Size){
  double aux;

  printf("\nAbrindo o arquivo.\n");
  FILE *filePointer = fopen("input.txt", "r");

  for(int i = 0; i < Size; i++){
    for(int j = 0; j < Size; j++){
      fscanf(filePointer, "%lf", &aux);
      Matrix[i*Size + j] = aux;
    }
    Vector[i] = Matrix[i*Size + i]/5;
  }
  
  printf("\nFim do arquivo.\n");

  fclose(filePointer);
}

void printMatrix(double* Matrix, int Size){
  printf("\n");
	for (int i=0; i<Size; i++){
    for (int j=0; j<Size; j++)
			printf("%lf ", Matrix[i * Size + j]);
    printf("\n");
  }
}

void printVector(double* Vector, int Size){
	for (int i=0; i<Size; i++)
		printf("%lf ", Vector[i]);
	printf("\n");
}

void ParallelGaussianElimination(double *Matrix, double *Vector, int Size){
  // Achar o maior pivot da iteração corrente(processo 0)
  double MaxValue; // Value of the pivot element of thе process
  int PivotPos = -1;    // Position of the pivot row in the process stripe

  // The iterations of the Gaussian elimination stage
  for (int i = 0; i < Size; i++){
    // Calculating the local pivot row
    if(ProcRank == 0){
      // printMatrix(Matrix, Size);
      // printVector(Vector, Size);
      double MaxValue = 0;
      for (int j = i; j < Size; j++){
        if (MaxValue < fabs(Matrix[j * Size + i])){
          MaxValue = fabs(Matrix[j * Size + i]);
          PivotPos = j;
        }
      }

      // Trocar de lugar com a linha corrente se for o caso(processo 0)
      if(PivotPos != -1){
        SwapRow(Matrix, i, PivotPos, Size);
        Swap(Vector, i, PivotPos, Size);
      }

      for (int j = i; j < Size; j++){
        Matrix[i * Size + j] = Matrix[i * Size + j]/MaxValue;
      }
      Vector[i] = Vector[i]/MaxValue;
      // printMatrix(Matrix, Size);
      // printVector(Vector, Size);
    }
    // printf("p(%d) \n", ProcRank);
    // printMatrix(Matrix, Size);
    // printVector(Vector, Size);
    // Atualizar Matrix e Vector
    MPI_Bcast(Matrix, Size * Size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(Vector, Size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // printf("p(%d) \n", ProcRank);
    // printMatrix(Matrix, Size);
    // printVector(Vector, Size);

    // Ir eliminando a linha paralelamente de acordo com o meu processo
    ParallelEliminateColumns(Matrix, Vector, i, Size);
  }

  printMatrix(Matrix, Size);
  printVector(Vector, Size);
}

void ParallelEliminateColumns(double *Matrix, double *Vector, int IndexPivot, int Size){
  printf("p(%d) \n", ProcRank);
  double multiplier;
  printf("inicio - %d, fim - %d\n", IndexPivot + (ProcRank*Size/ProcNum),IndexPivot + ((ProcRank+1)*Size/ProcNum));
  for (int i = IndexPivot + (ProcRank*Size/ProcNum); i < IndexPivot + ((ProcRank+1)*Size/ProcNum) && i < Size; i++){
    multiplier = Matrix[i * Size + IndexPivot] / Matrix[IndexPivot];
    for (int j = i+1; j < IndexPivot + ((ProcRank+1)*Size/ProcNum); j++){
      Matrix[i * Size + j] -= Matrix[j] * multiplier;
    }
    Vector[i] -= Vector[Size] * multiplier;
  }
}

void SwapRow(double* Matrix, int i, int j, int Size){
	//printf("Swapped rows %d and %d\n", i, j);

	for (int k=0; k<Size; k++){
		double temp = Matrix[i*Size + k];
		Matrix[i*Size + k] = Matrix[j*Size + k];
		Matrix[j*Size + k] = temp;
	}
}

void Swap(double* Matrix, int i, int j, int Size){
  double temp = Matrix[i];
	Matrix[i] = Matrix[j];
	Matrix[j] = temp;
}

void ParallelBackSubstitution(double *Matrix, double *Vector, double *Result, int Size){
  // Encontrar os valores do vetor resultado a partir do vector Vector e da matrix Matrix de forma paralela
}