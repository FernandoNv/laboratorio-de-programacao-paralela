#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int random_number(int min_num, int max_num){
  int result = 0, low_num = 0, hi_num = 0;

  if (min_num < max_num){
    low_num = min_num;
    hi_num = max_num + 1; // include max_num in output
  } else {
    low_num = max_num + 1; // include max_num in output
    hi_num = min_num;
  }

  srand(time(NULL));
  result = (rand() % (hi_num - low_num)) + low_num;
  return result;
}

int main(int argc, char** argv){
  FILE *filePointer;
  char fileName[100];
  scanf("%s", fileName);
  int count = 0;
  scanf("%d", &count);

  filePointer = fopen(fileName, "w");
  if( filePointer == NULL ){
    printf("%s file failed to open.", fileName);
  }else{  
    printf("The file is now opened.\n") ;
    
    for(int i = 0; i < count; i++){
      fprintf(filePointer,"%d",(rand()%count)+1);
      fputs("\n", filePointer);
    }
    // Closing the file using fclose()
    fclose(filePointer);
    printf("Data successfully written in file %s\n", fileName);
    printf("The file is now closed.") ;
  }

  return 0;
}
