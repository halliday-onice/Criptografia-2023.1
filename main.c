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
    char response = {0}; // esta setado pra nao ter lixo, vai que o lixo eh um n ou y? ai ferraria com o sistema
    //basicamente garante q nao seja nem y nem n

    

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
        while(response != 'y' && response != 'n'){
            //printf("\n");
            printf("Would you like to overwrite the file?[y/n]\n");
            scanf("%c",&response);
            printf("\n");

        } 
        
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
        uint8_t writeOnEncFile[16]; // precisamos guardar o texto cifrado pra poder escrever no arquivo o bloco de 16 bytes
    
        //printf("Antes do strcpy((char*)input,(char*)output)\n");
        strncpy((char*)input,(char*)output,16);
        //printf("input: %s\n",(char *)input);
        //printf("output: %s\n",(char *)output);
        int i = fileSize;
        while(1){ //tem um bloco pra criptografar? porque cada bloco possui 16 bytes
            printf("i: %d\n",i);
        
            if(i < 16){ // precisamos ver se vai sobrar blocos menores que 16 bytes , eh isso q esse if esta fazendo
                //imagina que tenho um arquivo de 18 bytes
                // na segunda interacao o i = 2(18 -16)
                // como 2 < 16 eu estou na verdade guardando a quantidade de bytes do ultimo bloco
                keepRemain = i; // estou guardando essa variavel para ser usado no padding - pra saber quanto vamos ter q adicionar no padding
                break; // se chegamos em um i menor q 16 ou seja - tem menos de um bloco - ele para
            }

            puts("Entrou no for");
            // precisamos comecar a criptografar
            AES128_Encrypt(input,key,output);
            puts("saiu do AES");
            strncpy((char*)input,(char*)output,16); // nos estamos preparando o input pra proxima interacao, o output vai ser o input da proxima interacao
            puts("Antes do fread, chegou?");
            // como estamos copiando o output no input, em fread nos podemos usar o output - onde vamos guardar(o buffer, basicamente)
            fread(output,1,16,inputFile);

            XorBlock(writeOnEncFile,input,output); // o output ficou nessa posicao do text porque  dps de copiarmos o valor pro input ele ficou como o input
            fwrite(writeOnEncFile,1,16,outputFile);

            i-= 16;

            
        }
        printf("Keep Remain: %d\n",keepRemain);
        if(keepRemain > 0){
            // TO DO padding
        }

        if(keepRemain == 0){
            // se for 0 o arquivo eh multiplo de 16
        }
    
    }

}

char *createKey(){
        struct termios term;
        // Get the current terminal settings
        tcgetattr(STDIN_FILENO, &term);
        
        // Turn off echoing of typed characters - desliga o comando echo do terminal
        term.c_lflag &= ~(ECHO);

        // Set the new terminal settings - configura pro terminal nao mostrar mais
        tcsetattr(STDIN_FILENO, TCSANOW, &term);

        //pegamos a senha aqui
        char *password = calloc(sizeof(char),16); // estamos separando 16 bytes
        printf("Enter password: ");
        scanf("%s", password);

        // Restaura as configs do terminal

        // Restore the original terminal settings
        term.c_lflag |= ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &term);

        return password;

        //printf("Password: %s\n", password);
        puts("\n");
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
        printf("mode: OFB padding: ISO/IEC 7816-4:2005\n");
        if(strcmp("enc",argv[1]) == 0){

            strncpy((char*)key,createKey(),16);
            
            EncOFB(IVec,key,argv[2],argv[4]);
        } else{
            printf("File does not exist\n");
            return 1;
        }
    

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
       

