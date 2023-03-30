#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 256
#define KEY_LENGTH 128  // length of the key in bytes




void swap(unsigned char *a, unsigned char *b)
{     
      int temp = *a;
      *a = *b;
      *b = temp;
      printf("%s\n",a);

}
// KSA is responsible for the permutation
void KSA(char *key, unsigned char *S)
{ 
      //S eh o vetor temporario do livro
      int j;
      int keysize= strlen(key);

      for(int i =0; i < N;i++)
            S[i] = i;

      for(int i =0; i < N;i++){
            j = (j + S[i] + key[i % keysize]) % N;

            swap(&S[i],&S[j]);

      }
      


}

// This part is the "geracao de fluxo" part
// In which we 

void PRGA(unsigned char *S,char *plaintext,unsigned char *ciphertext)
{
      int i = 0;
      int j = 0;
      int tam_plaintext = strlen(plaintext);

      for(int i = 0; i < tam_plaintext;i++){
            i = (i + 1) % N;
            j = (j + S[i]) % N;

            swap(&S[i],&S[j]);
            int rndom = S[(S[i] + S[j]) % N];

            ciphertext[i] = rndom ^ plaintext[i];
      }
}

void generateRKey(char *key){
      srand(time(NULL));

      for(int i =0; i < KEY_LENGTH; i++){ //16 bits for the key
            key[i] = rand() % N;

      }
      
      

}


void RC4(char *key,char *plaintext,unsigned char *ciphertext){

      generateRKey(key);

      unsigned char S[N];

      KSA(key,S);

      PRGA(S,plaintext,ciphertext);




}


int main(int argc, char *argv[])
{
      // FILE *fp = fopen("teste1",r+); // r+ stands for both read and write
      // if(argc < 2){
      //       printf("ERROR\n");
      //       return -1;
      // }

      char key[KEY_LENGTH];

      generateRKey(key);

      for (int i = 0; i < KEY_LENGTH; i++) {
        printf("%02X", key[i]);
      }
      printf("\n");

      unsigned char *ciphertext = malloc(sizeof(int) * strlen(argv[1]));

      

      //printf("seedKey %c\n",seedKey);

      //RC4(&seedKey,argv[1],ciphertext);

      // int tamText = strlen(argv[1]);

      // for(int i = 0;i < tamText;i++){
      //       printf("%02X",ciphertext[i]);
      // }




      //unsigned char a = "Ahh";
      

      return 0;
}