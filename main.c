#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <termios.h>

#include "aes.h"

// Essa funcao verifica a existencia do arquivo que estamos querendo criptografar 
// ela retorna um inteiro, uma flag. Ela usa a systemCall access
// 
int VerifyFileExist(char *fileName){
      if(access(fileName, F_OK) == 0)
            return 0; // aquivo existe
      else
            return 1;

}

// esse XORBlock existe pq ao final da cipher encryption - ou seja
// dps de passar pelo AES_128 vamos ter um bloco de 16 bytes criptografado - por isso o block do xorBlock
void XorBlock(uint8_t *outputWriteFile, uint8_t *output, uint8_t *text){
    for(int i =0;i < 16; i++){
        outputWriteFile[i] = output[i] ^ text[i];
    }
}

void EncOFB(uint8_t Ivec[16],uint8_t key[16],char *fileNameInput,char *fileNameOutput){
    FILE *outputFile;
    FILE *inputFile;
    char response = {0};

    

    inputFile = fopen(fileNameInput, "rb");


// Nesse trecho abaixo nos estamos verificando se o arquivo de output, ou seja aquele q eu quero escrever
// existe. 
//    Se ele existir
//    vamos perguntar se gostaria de sobrescrever, se nao quiser simplesmente da exit no programa
//    se quiser o programa vai seguir seu curso e o outputFile = fopen(fileNameOutput,"wb") vai sobrescrever
// 
// 
//     
    if(VerifyFileExist(fileNameOutput) == 0 ){
        do{
            printf("Would you like to overwrite the file?[y/n]\n");
            scanf("%c",&response);

        } while(response != 'y' && response != 'n');
        
        if(response == 'n')
            exit(1);

    }

    outputFile = fopen(fileNameOutput,"wb");
        
    if(inputFile == NULL)
        printf("Could not open the file, it does not exist.\n");
    else
    {
    
        //comeca pelo inicio do arquivo e procura pelo ponteiro do fim(SEEK_END) 
        fseek(inputFile, 0L, SEEK_END);
        int fileSize = ftell(inputFile);
        printf("SIZE OF THE FILE IN BYTES IS: %d\n",fileSize);

        fseek(inputFile, 0L, SEEK_SET); // Coloca o ponteiro para o inicio do arquivo novamente


        // Precisamos saber a quantidade de blocos que o arquivo vai ter
        float blockSize = fileSize/16; // pra ver quantas interacoes no loop vamos fazer

        printf("BLOCKS: %f\n", ceil(blockSize));

        int keepRemain = 0; 
        // esse keepremain vai guardar o quanto de bytes faltam pra chegarmos 
        //ate o fim do arquivo
        uint8_t output[16];
        uint8_t input[16];
            

        strcpy((char*)input,(char*)output);
        for(int i =fileSize; i >= 16;i -=16){ //tem um bloco pra criptografar? porque cada bloco possui 16 bytes
                
            // precisamos comecar a criptografar 
            AES128_Encrypt(input,key,output);
            strcpy((char*)input,(char*)output);
                
            if(i < 16){ // precisamos ver se vai sobrar blocos menores que 16 bytes , eh isso q esse if esta fazendo
                //imagina que tenho um arquivo de 18 bytes
                // na segunda interacao o i = 2(18 -16)
                // como 2 < 16 eu estou na verdade guardando a quantidade de bytes do ultimo bloco
                keepRemain = i; // estou guardando essa variavel para ser usado no padding - pra saber quanto vamos ter q adicionar no padding
            }

        }
      

    }

}

int main(int argc, char **argv) {

    //printf("ARGC: %d\n",argc);
    if(argc != 5){
        printf("Error! Please write 4 inputs\n");
        return 1;
        
    }

    uint8_t key[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };    // 16 bytes = 128 bits
    uint8_t IVec[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // inicializar o input vector com zero
    uint8_t input[16];
    uint8_t output[16];
    uint8_t dec[16]; 
 
    
    
    int fileok = VerifyFileExist(argv[2]);

    if(fileok == 0)
    { // vendo se o arquivo existe
        if(strcmp("enc",argv[1]) == 0){
            // esse FOR existe porque queremos criptografar em blocos
            // quando chegar no ULTIMO BLOCO nos utilizaremos o padding especificado
            //for(int i = 0;i < blockSize; i++ ){
            //   fread(input, 1, 16,inputFile);
                //AES128_Encrypt()
            //}
                  
            //fclose(inputFile);
        } else{
            printf("File does not exist\n");
            return 1;
        }
    

    

        printf("mode: OFB padding: ISO/IEC 7816-4:2005\n");

        struct termios term;

        // Get the current terminal settings
        tcgetattr(STDIN_FILENO, &term);

        // Turn off echoing of typed characters - desliga o comando echo do terminal
        term.c_lflag &= ~(ECHO);

        // Set the new terminal settings - configura pro terminal nao mostrar mais
        tcsetattr(STDIN_FILENO, TCSANOW, &term);

        //pegamos a senha aqui
        char password[150];
        printf("Enter password: ");
        scanf("%s", password);

        // Restaura as configs do terminal

        // Restore the original terminal settings
        term.c_lflag |= ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &term);

        //printf("Password: %s\n", password);
        puts("\n");
    }

    
    // printf("Enter with message: ");
    // fgets((char*)input, 16, stdin ); // need to cast uchar to char to work
    
    
    // printf("Enter passphrase for key: ");
    // fgets((char*)key, 16, stdin );
    
    // AES128_Encrypt(input, key, output);
    // AES128_Decrypt(output, key, dec);
    
    // printf("\n");
    // printf("Encrypted message: \n");
    // for( int i = 0; i < 16; i++ ) {
    //     printf("%.2X", output[i] );
    // }
    // printf("\n");
    // printf("Decrypted message: \n");
    // printf("%s\n", dec);
    
    return 0;

}
       

