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
    
    // Nesse trecho abaixo nos estamos verificando se o arquivo de output, ou seja aquele q eu quero escrever
    // existe. 
    //    Se ele existir
    //    vamos perguntar se gostaria de sobrescrever, se nao quiser simplesmente da exit no programa
    //    se quiser o programa vai seguir seu curso e o outputFile = fopen(fileNameOutput,"wb") vai sobrescreve
   
    if(VerifyFileExist(fileNameOutput) == 0 ){
        
        while(response != 'y' && response != 'n'){
            //printf("\n");
            printf("Would you like to overwrite the file?[y/n]\n");
            scanf("%c",&response);
            fpurge(stdin); // limpando o buffer de response
            printf("\n");

        } 
        
        if(response == 'n')
            exit(1);

    }
    inputFile = fopen(fileNameInput, "rb");
    outputFile = fopen(fileNameOutput,"wb");
        
    if(inputFile == NULL)
        printf("Could not open the file, it does not exist.\n");
    else
    {
    
        //comeca pelo inicio do arquivo e procura pelo ponteiro do fim(SEEK_END) 
        fseek(inputFile, 0L, SEEK_END);
        unsigned long int fileSize = ftell(inputFile);
        //printf("SIZE OF THE FILE IN BYTES IS: %ld\n",fileSize);

        fseek(inputFile, 0L, SEEK_SET); // Coloca o ponteiro para o inicio do arquivo novamente


        int keepRemain = 0; 
        // esse keepremain vai guardar o quanto de bytes faltam pra chegarmos 
        //ate o fim do arquivo
        uint8_t plaintextBlock[16] =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        uint8_t output[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        uint8_t input[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        uint8_t writeOnEncFile[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // precisamos guardar o texto cifrado pra poder escrever no arquivo o bloco de 16 bytes
    
        //printf("Antes do strcpy((char*)input,(char*)output)\n");
        strncpy((char*)input,(char*)Ivec,16);
        //printf("input: %s\n",(char *)input);
        //printf("output: %s\n",(char *)output);
        unsigned long int i = fileSize;
        while(1){ //tem um bloco pra criptografar? porque cada bloco possui 16 bytes
            //printf("i: %ld\n",i);
        
            if(i < 16){ // precisamos ver se vai sobrar blocos menores que 16 bytes , eh isso q esse if esta fazendo
                //imagina que tenho um arquivo de 18 bytes
                // na segunda interacao o i = 2(18 -16)
                // como 2 < 16 eu estou na verdade guardando a quantidade de bytes do ultimo bloco
                keepRemain = i; // estou guardando essa variavel para ser usado no padding - pra saber quanto vamos ter q adicionar no padding
                break; // se chegamos em um i menor q 16 ou seja - tem menos de um bloco - ele para
            }

            //puts("Entrou no for");
            // precisamos comecar a criptografar
            AES128_Encrypt(input,key,output);
            //puts("saiu do AES");
            strncpy((char*)input,(char*)output,16); // nos estamos preparando o input pra proxima interacao, o output vai ser o input da proxima interacao
            //puts("\n");
            //printf("input: %s\n",(char *)input);
            //puts("Antes do fread, chegou?\n");
            // como estamos copiando o output no input, em fread nos podemos usar o output - onde vamos guardar(o buffer, basicamente)
            fread(plaintextBlock,1,16,inputFile);
            XorBlock(writeOnEncFile, plaintextBlock, output); // o output ficou nessa posicao do text porque  dps de copiarmos o valor pro input ele ficou como o input
            //printf("Depois do XOR\n");
            fwrite(writeOnEncFile,1,16,outputFile);
            //printf("Write on block: %s\n",(char*)writeOnEncFile);

            i-= 16;

            
        }   
        //printf("Keep Remain: %d\n",keepRemain);
        if(keepRemain > 0){
            //puts("ENTROU no keepremain > 0");
            // nao era um arquivo com um numero de bytes multiplo de 16, ai temos q fazer o padding
            // temos keepremain posicoes preenchidas - no exemplo de 25 bytes, temos 9 posicoes preenchidas 
            fread(plaintextBlock,1,keepRemain,inputFile); // to lendo de um em um byte até o keepremain e vou colocar isso no buffer output
            plaintextBlock[keepRemain] = 0x80; // preenchendo com o 80 em hexadecimal -> q eh 128 em decimal -  como no slide 
            //printf("ULTIMO BLOCO ANTES DO WHILE: %x\n",plaintextBlock[keepRemain]);
            while(keepRemain < 15){ // preciso avancar ate o final do bloco de 16 bytes - vai ate 15 pq em C comecamos na posicao 0
                // nesse while eu to na casa onde comeca o padding e preciso avancar ate o fim do bloco
                keepRemain++; // estamos indo pra proxima posicao
                plaintextBlock[keepRemain] = 0x00;
                //printf("ULTIMO BLOCO FIM DO WHILE: %x\n",plaintextBlock[keepRemain]);
            }
           

        }

        if(keepRemain == 0){
            //precisamos adicionar um novo bloco
            //puts("ENTROU no keepremain == 0");
            // se o arquivo for multiplo de 16 - vamos adicionar um bloco novo para AI SIM podermos fazer o padding
            // vamos fazer um bloco só de padding
            uint8_t newBlock[16] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // traduzindo estamos colocando 16 bytes a mais
            strncpy((char*)plaintextBlock,(char*)newBlock,16); // copia para o novo conteudo de newblock -> agr o output vai ser IGUAL ao newBlock, que eh justamente um novo bloco INTEIRAMENTE NOVO CRIADO

        }
        //printf("ULTIMO BLOCO CHAR A CHAR\n");
        // for(int i =0;i < 16;i++){
        //     printf("%x\n",plaintextBlock[i]);
        // }
        // no trecho acima nos completamos com o padding
        // e agr temos um bloco multiplo 16 e ai podemos seguir com o esquema da criptografia
        //puts("COMECA A ULTIMA CRIPTOGRAFIA");
        
        AES128_Encrypt(input, key, output); // criptografa o ultimo bloco
        XorBlock(writeOnEncFile, plaintextBlock, output);
        fwrite(writeOnEncFile,1,16,outputFile);

        puts("CRIPTOGRAFADO!!!!"); // testamos com um arquivo de tamanho 25  E ele acabou criando um arquivo de tamanho 32, ou seja , 8 bytes a MAIS- por conta do 0x80
    
    }
    fclose(inputFile);
    fclose(outputFile);

}



// 
// Essa funcao vai servir para descriptografar o arquivo !
// enquanto nao for o ultimo bloco a gnt descriptografa e escreve...(e fica nesse ciclo) até chegar ao ultimo bloco
// NO ultimo bloco vamos ler o conteudo do arquivo criptografado, vmos pegar esse conteudo e descriptografar
// antes de escrever no arquivo, vamos verificar o conteudo pra tirar o padding
// 
// 
// 

void DecOFB(uint8_t Ivec[16],uint8_t key[16],char *fileNameInput,char *fileNameOutput){
    FILE *outputFile;
    FILE *inputFile;
    char response = {0}; // esta setado pra nao ter lixo, vai que o lixo eh um n ou y? ai ferraria com o sistema

    if(VerifyFileExist(fileNameOutput) == 0 ){
        while(response != 'y' && response != 'n'){
            //printf("\n");
            printf("Would you like to overwrite the file?[y/n]\n");
            scanf("%c",&response);
            fpurge(stdin);
            printf("\n");

        } 
        if(response == 'n')
            exit(1);

    }

    inputFile = fopen(fileNameInput, "rb"); // abrindo para leitura
    outputFile = fopen(fileNameOutput,"wb"); // abrindo para escrita
        
    if(inputFile == NULL)
        printf("Could not open the file, it does not exist.\n");
    else
    {
    
        //comeca pelo inicio do arquivo e procura pelo ponteiro do fim(SEEK_END) 
        fseek(inputFile, 0L, SEEK_END);
        unsigned long int fileSize = ftell(inputFile);
        //printf("SIZE OF THE FILE IN BYTES IS: %ld\n",fileSize);

        fseek(inputFile, 0L, SEEK_SET); // Coloca o ponteiro para o inicio do arquivo novamente
        // esse keepremain vai guardar o quanto de bytes faltam pra chegarmos 
        //ate o fim do arquivo
        uint8_t ciphertextBlock[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        uint8_t output[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        uint8_t input[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        uint8_t writeOnDecFile[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // precisamos guardar o texto cifrado pra poder escrever no arquivo o bloco de 16 bytes
    
        //printf("Antes do strcpy((char*)input,(char*)output)\n");
        strncpy((char*)input,(char*)Ivec,16); // se olharmos bem o PDF da aula, o decrypt será também com encrypt e estamos copiando o valor de IVec para input, dentro do while nos fazemos a alteracao correta, que eh copiar o output no input
        //printf("input: %s\n",(char *)input);
        //printf("output: %s\n",(char *)output);
        unsigned long int i = fileSize;
        while(i > 16){
            // > 16 pq estamos trabalhando com 16 bytes.  
            // vamos precisar mexer no ultimo bloco pra tirar o padding, aqui no decrypt
            //printf("i: %ld\n",i);
        
            //puts("Entrou no for");
            // precisamos comecar a criptografar
            AES128_Encrypt(input,key,output);
            //puts("saiu do AES");
            strncpy((char*)input,(char*)output,16); // nos estamos preparando o input pra proxima interacao, o output vai ser o input da proxima interacao
            //puts("\n");
            //printf("input: %s\n",(char *)input);
            //puts("Antes do fread, chegou?\n");
            // como estamos copiando o output no input, em fread nos podemos usar o output - onde vamos guardar(o buffer, basicamente)
            fread(ciphertextBlock,1,16,inputFile);
            XorBlock(writeOnDecFile,ciphertextBlock,output); // o output ficou nessa posicao do text porque  dps de copiarmos o valor pro input ele ficou como o input
            //printf("Depois do XOR\n");
            fwrite(writeOnDecFile,1,16,outputFile);
            //printf("Write on block: %s\n",(char*)writeOnDecFile);

            i-= 16; // vamos descriptografar bloco a bloco, por isso o i-=16 vai continuar

            
        }
        // estamos no ultimo bloco
        AES128_Encrypt(input,key, output); // no modo OFB aqui eh ENCRYPTION MESMO- olhar no slide
        fread(ciphertextBlock,1,16,inputFile);
        XorBlock(writeOnDecFile, ciphertextBlock, output); // o fread vai guardar no ciphertext
        
        // precisamos saber ate onde escrever no bloco byte a byte de tras pra frente
        // basicamente estamos querendo tirar o padding
        // inicializamos uma variavel do tamanho maximo do vetor - que eh 15, e
        // ao olhar o esquema desenhado pelo rapha, no while ele vai
        // ficar testando se na posicao verifyLen = 0x80 ou seja, eh nessa posicao que inicia o padding
        // se for, a gente da break e sai do while, 
        // caso contrario decrementamos o contador pra ir de tras pra frente
        int VerifyLen = 15; // o tamanho max do vetor eh 15 - vetor comeca no zero
        while(VerifyLen){ // aqui estamos lendo de tras pra frente
            //printf("ULTIMO BLOCO: %x\n",writeOnDecFile[VerifyLen]);
            if(writeOnDecFile[VerifyLen] == 0x80){
                break;
            }
            VerifyLen--;
            

        }
        //printf("SAI DO WHILE NO DECRYPT, ULTIMO BLOCO: %x\n",writeOnDecFile[VerifyLen]);

        // se verifylen for diferente de zero signca que o bloco tem conteudo e padding e so vamos escrever no writeondecfile o conteudo ate verifyLen bytes
        if(VerifyLen != 0) // so escreve no novo arquivo decriptografado se nao for zero
            fwrite(writeOnDecFile,1,VerifyLen,outputFile); // estamos escrevendo a quantidade de bytes excluindo o padding, por isso estamos usando o tamanho VerifyLen
        // se nao for diferente de zero signifca que o bloco eh so de padding, ou seja, ele foi "completado" e nao tem conteudo
        
        fclose(inputFile);
        fclose(outputFile);

        puts("DESCRIPTOGRAFADO!!!!");
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
        // vamos testar se ao setar uma chave funciona - 31st march 2023
        //char password[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        //pegamos a senha aqui
        char *password = calloc(sizeof(char),16); // estamos separando 16 bytes
        printf("Enter password: ");
        scanf("%s", password);
        fpurge(stdin); // estamos limpando o buffer da entrada - se nao tiver isso  aqui ele fica printando duas vezes o would you like to overwrite
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
        printf("\n");
        printf("mode: OFB padding: ISO/IEC 7816-4:2005\n");
        if(strcmp("enc",argv[1]) == 0){

            strncpy((char*)key, createKey(), 16);
            
            EncOFB(IVec,key,argv[2],argv[4]);
        }
        else if(strcmp("dec",argv[1]) == 0){

            strncpy((char*)key, createKey(), 16);

            DecOFB(IVec,key,argv[2],argv[4]);

        } else{
            printf("File does not exist\n");
            return 1;
        }
    
    }

    return 0;
}
