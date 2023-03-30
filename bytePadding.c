#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "aes.h"


int VerifyFileExist(char *fileName){
      if(access(fileName, F_OK) == 0)
            return 0; // aquivo existe
      else
            return 1;

}


int main(){
      char ch;
      char fileName[50];
      FILE *f;
      
      printf("Enter the file name:");
      scanf("%s",fileName);

      int fileok = VerifyFileExist(fileName);

      if(fileok == 0)
      {

            f = fopen(fileName, "r+");
            

            if(f == NULL)
                  printf("Could not open the file.\n");
            else
            {
                  printf("The contents of the file %s are:\n",fileName);

                  while((ch = fgetc(f)) != EOF)
                        printf("%c",ch);
                  printf("\n");
                  
                  fclose(f);
            
            }

      } else{
            printf("File does not exist\n");
      }




      return 0;
}