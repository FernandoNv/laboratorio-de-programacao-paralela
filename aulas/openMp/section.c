#include <stdio.h>
#include <omp.h>

float f(int x, int IT){
  int k = x;
  while (k < IT) k++;
  return (k);
}

float g(int x, int IT){
  int res = x;
  for (int k = 0; k < IT; k++);
  return (res);
}

int main(int argc, char **argv){
  // int iter = atoi(argv[1]);
  int iter = 1000;
  float result = f(2, iter) + g(2, iter);

  printf("%.2f \n", result);

  return 0;
}