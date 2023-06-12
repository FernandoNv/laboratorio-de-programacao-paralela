#include <stdio.h>
#include <omp.h>

int main(int argc, char *argv[]){
  int i, myid, neighb, a[4] = {1, 2, 3, 4}, b[4] = {0, 0, 0, 0};
  int c = 2;
  #pragma omp parallel private(i, myid, neighb) num_threads(4)
  {
    myid = omp_get_thread_num();
    neighb = myid - 1;

    if(myid == 0) neighb = omp_get_num_threads() - 1;

    a[myid] = a[myid]*3;
    #pragma omp barrier
    {
      b[myid] = a[neighb] + c;
    }

    // b[myid] = a[neighb] + c;

    printf("%i - valor: %i \n", myid, b[myid]);
  }

  return 0;
}