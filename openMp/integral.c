#include <stdio.h>
#include <omp.h>
#include <time.h>

#define NUM_THREADS 4

float calcula(float local_a, float local_b, int local_n, float h) {
  float integral;
  float x, i;
  float f(float x); // função a integrar
  integral = (f(local_a) + f(local_b)) /2.0;

  x = local_a;
  for( i=1; i<=local_n; i++) {
    x += h;
    integral += f(x);
  }

  integral *= h;
  return integral;
}

float f(float x) {
  float fx; // valor de retorno
  // esta é a função a integrar
  // exemplo: função quadrática
  fx = x * x;

  return fx;
}

int main(int argc, char** argv){
  time_t begin = time(NULL);
  int i;
  double pi, sum[NUM_THREADS], integral;
  int nthreads;

  omp_set_num_threads(NUM_THREADS);
  #pragma omp parallel
  {
    int id, n=1024, local_n, nthrds, resto;
    float integral_part, h, local_a, local_b, a=0.0, b=1.0;
    nthrds = omp_get_num_threads();
    if (id == 0) nthreads = nthrds;
    id = omp_get_thread_num();
    
    h = (b-a) / n;
    local_n = n / nthrds;
    local_a = a + id * local_n * h;
    local_b = local_a + local_n * h;
    // somando o resto no final
    if(id == nthrds-1){
      local_n = (n/nthrds) + (n%nthrds);
      local_b = b;
    }

    //balanceamento
    resto = n%nthrds;
    if(resto != 0){
      if(id < resto) local_n = local_n + 1;
      local_a = a + id * local_n * h;
      local_b = local_a + local_n * h;
    }
    sum[id] = calcula(local_a, local_b, local_n, h);
  }

  for(i = 0; i < nthreads; i++) integral += sum[i];
  printf("%f \n", integral);
  time_t end = time(NULL);
  printf("Tempo: %ld \n", (end-begin));
  return 0;
}