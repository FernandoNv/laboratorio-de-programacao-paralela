# laboratorio-de-programacao-paralela
Arquivos da matéria TCC00344 - LABORATÓRIO DE PROGRAMAÇÃO PARALELA I. Códigos em C utilizando OpenMP e MPI

## Breve tutorial de instalação do Open MPI em uma distribuição linux(Ubuntu)

Passo 1) Abrir o terminal (Ctrl + Alt + T)

Passo 2) Digitar e rodar no terminal: sudo add-apt-repository universe

Passo 3) Digitar e rodar no terminal: sudo apt install libopenmpi-dev

Passo 4) Digitar e rodar no terminal para verificar se a instalação foi bem sucedida (retorna a versão instalada) : mpiexec --version
A princípio, irá retornar o seguinte texto: mpiexec (OpenRTE) 2.1.1

## Compilando e executando MPI:
- para compilar: mpicc seuPrograma.c -o seuPrograma
- para executar: mpirun -n X SeuPrograma ou mpiexe -np X teste
### Onde X é o número de processos. Como não está sendo especificado um "hostfile" com uma lista de hosts e slots, o seu programa executará localmente com aquele número de processos.

## Compilando e executando OpemMPI
- Fora do programa, pode ser especificado o número de threads que serão disparadas, com o uso da variável de ambiente OMP_NUM_THREADS
- Dentro do programa, podem ser utilizadas as seguintes opções: omp_numthreads(n), omp_set_num_threads(n)
- Compilando 
 gcc omp_hello.c -fopenmp -o teste
- Executando
 ./teste