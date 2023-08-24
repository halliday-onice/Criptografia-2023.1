#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>


#define uchar unsigned char 
#define SBOXLEN 256
#define INPUTBYTES 8
uchar SBOX[256] = {129, 249, 58, 80, 242, 68, 86, 195, 60, 154, 152, 37, 189, 150, 219, 160, 231, 131, 218, 50, 6, 217, 36, 125, 252, 81, 147, 153, 119, 185, 122, 57, 42, 52, 173, 87, 111, 182, 8, 84, 10, 193, 194, 228, 89, 12, 105, 198, 40, 25, 14, 223, 156, 126, 45, 43, 18, 190, 133, 102, 229, 158, 132, 72, 5, 235, 29, 108, 65, 75, 239, 179, 222, 248, 210, 196, 151, 141, 103, 3, 78, 180, 115, 28, 41, 24, 53, 11, 82, 0, 134, 20, 77, 83, 94, 214, 117, 255, 148, 48, 85, 234, 159, 51, 136, 215, 71, 174, 175, 33, 227, 35, 224, 7, 253, 113, 145, 99, 233, 161, 164, 98, 92, 23, 32, 155, 205, 101, 221, 106, 59, 64, 116, 62, 144, 139, 178, 247, 241, 220, 142, 211, 34, 67, 27, 168, 120, 188, 21, 138, 110, 88, 69, 4, 232, 170, 56, 44, 244, 124, 73, 90, 30, 250, 46, 123, 165, 49, 183, 209, 76, 47, 163, 226, 246, 55, 177, 140, 207, 184, 208, 191, 187, 22, 118, 143, 236, 146, 167, 2, 197, 245, 112, 181, 166, 162, 104, 192, 26, 172, 38, 186, 206, 251, 61, 216, 93, 9, 240, 16, 19, 203, 127, 238, 95, 15, 200, 63, 91, 213, 254, 31, 17, 230, 157, 225, 201, 130, 1, 149, 96, 70, 100, 97, 121, 66, 137, 212, 171, 74, 176, 202, 204, 237, 54, 114, 13, 128, 243, 79, 109, 39, 135, 169, 199, 107};
//uchar SBOX[16] = { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 };

int equations[9][9];

uchar bit( uchar x, uchar i ) {
    return (x >> (8-i)) & 1;
}


uchar EBlock( uchar w, uchar k ) {
    uchar x = w ^ k;
    uchar y = SBOX[x];
    return y;
}

uchar* E(uchar *w,uchar *k,int wlen){
    uchar *wc = (uchar*)calloc(wlen,sizeof(uchar));
    for(int i = 0;i < wlen;i++){
        wc[i] = EBlock(w[i],k[i % 4]);
    }
    return wc;
    
}


void getAbsValue(int **biasTable){
    int maxValue = 0;
    int rowMax;
    int columnMax;
    int x1,x2,x3,x4,x5,x6,x7,x8;
    int y1,y2,y3,y4,y5,y6,y7,y8;
    int counter = 1;
    
    for(int i = 1; i < 256 ; i = i * 2){
        maxValue = 0;
        rowMax = 0;
        columnMax = 0;
        int teste = 0;
        for(int j = 1; j < 256; j++){// j eh o y da tabela de bias

            if(abs(biasTable[i][j]) > maxValue){
                maxValue = abs(biasTable[i][j]);
                teste = biasTable[i][j];
                rowMax = i; //por conta do primeiro for onde ja ocorreu a filtragem
                columnMax = j;
            }
            
        }
        printf("teste: %d\n",teste);
        
       
        equations[counter][0] = biasTable[rowMax][columnMax] < 0 ? 1: 0;
        for(int y = 1;y < 9;y++){
            equations[counter][y] = bit(columnMax,y);

        }
        counter++;

            
            
        
         /* x1 = bit(rowMax, 1);
        x2 = bit(rowMax, 2);
        x3 = bit(rowMax, 3);
        x4 = bit(rowMax, 4);
        x5 = bit(rowMax, 5); 
        x6 = bit(rowMax, 6);
        x7 = bit(rowMax, 7);
        x8 = bit(rowMax, 8); 
        y1 = bit(columnMax, 1); 
        y2 = bit(columnMax, 2); 
        y3 = bit(columnMax, 3); 
        y4 = bit(columnMax, 4); 
        y5 = bit(columnMax, 5); 
        y6 = bit(columnMax, 6); 
        y7 = bit(columnMax, 7); 
        y8 = bit(columnMax, 8);
        printf("X: %d, x1: %d ,x2: %d, x3: %d, x4: %d, x5: %d, x6: %d, x7: %d, x8: %d\n",rowMax,x1,x2,x3,x4,x5,x6,x7,x8);
        printf("Y : %d, y1: %d ,y2: %d, y3: %d, y4: %d, y5: %d, y6: %d, y7: %d, y8: %d\n",columnMax,y1,y2,y3,y4,y5,y6,y7,y8);

        */

    }
    printf("MAXVALUE: %d\n",maxValue);
    printf("linhaMax: %d\n",rowMax);
    printf("colunaMax: %d\n",columnMax);
    
}
void genBiasTable() {
    int **biasTable = (int**)malloc(sizeof(int*)*SBOXLEN); //gonna save the bias information here
    
    for(int j = 0; j < 256; j++){ // esse for aqui eh para colunas
        biasTable[j] = (int*) malloc(sizeof(int*) * SBOXLEN);
    }
    
    int alpha = 0;
    for( int x = 0; x < SBOXLEN; x++ ) {
        printf("c|");
    }
    printf("\n & ");
    for( int x = 1; x < SBOXLEN; x++ ) {
        if( x != SBOXLEN - 1 ) {
            printf(" {\\bf %.1X} &", x);
        } else {
            printf(" {\\bf %.1X} \\\\\\hline\n", x);
        }
    }
    
    for(int x = 1; x < SBOXLEN; x++ ) {
        printf(" {\\bf %.1X} &", x);
        for( int beta = 1; beta < SBOXLEN; beta++ ) {
            
            alpha = 0;
            for(int X = 0; X < SBOXLEN; X++ ) {
                int a;
                if((a=((bit(X, 1)&bit(x, 1)) ^ 
                    (bit(X, 2)&bit(x, 2)) ^
                    (bit(X, 3)&bit(x, 3)) ^
                    (bit(X, 4)&bit(x, 4)) ^
                    (bit(X, 5)&bit(x, 5)) ^
                    (bit(X, 6)&bit(x, 6)) ^
                    (bit(X, 7)&bit(x, 7)) ^
                    (bit(X, 8)&bit(x, 8)) ^
                    (bit(SBOX[X], 1)&bit(beta, 1)) ^
                    (bit(SBOX[X], 2)&bit(beta, 2)) ^
                    (bit(SBOX[X], 3)&bit(beta, 3)) ^
                    (bit(SBOX[X], 4)&bit(beta, 4)) ^
                    (bit(SBOX[X], 5)&bit(beta, 5)) ^
                    (bit(SBOX[X], 6)&bit(beta, 6)) ^
                    (bit(SBOX[X], 7)&bit(beta, 7)) ^
                    (bit(SBOX[X], 8)&bit(beta, 8)))) == 0 )       
                        alpha++;
            }
            //printf("alfa: %d\n",alpha);
            biasTable[x][beta] =  alpha - 128;
            //printf(">=>=>= %d\n", alpha - 8);       
            if( beta != SBOXLEN - 1) {
                printf("%d & ", biasTable[x][beta]); // 2ˆ4 - 1
            } else {
                printf("%d\\\\\\hline\n", biasTable[x][beta]);
            }        
        }
    }
    getAbsValue(biasTable);

    for(int x = 1; x< 9 ;x++){
        printf("k%d = w%d",x,x);
        for(int y = 1;y < 9;y++){
            //printf(" %d ",equations[x][y]);
            if(equations[x][y] == 1){
                printf(" XOR y%d", y);
                //printf(" ^ bit(Y,%d)", y);

            }
                
        }
        if(equations[x][0] == 1){
            printf(" XOR 1");
        }
        printf("\n");
        printf("if(");
        for(int y = 1;y < 9;y++){
            //printf(" %d ",equations[x][y]);
            if(equations[x][y] == 1){
                printf(" ^ bit(Y,%d)", y);
            }
                
        }
        if(equations[x][0] == 1){
            printf(" ^ 1");
        }
        printf(" == 1) k%d++",x);
        printf("\n");
            
            
    }
    
}




int main() {
  /*int c = 0;
  uchar Y = 0;
  for( uchar X = 0; X < 16; X++ ) {
    Y = SBOX[X];
    if( bit(X, 3)^ bit(X, 4)^ bit(Y, 1)^bit(Y, 4) == 1 )       
        c++;
  }
  printf("c = %d\n", c);  
  */ 
  
  genBiasTable();
  
  float keypart[5][9];

  srand( time(NULL) );
  uchar Kp[4]= {1, 3, 5, 8};
  int c = 0;
  int N = 1000000;
  uchar Wp[N];
  
  for(int i = 0; i < N; i++ )
    Wp[i] = rand() & 0x100; // gerando um texto plano aleatorio de 1 byte
  uchar *Wc = E(Wp,Kp,N); //TEXTO ECRYPT

  for(int i = 0; i < N; i++ ){
    int K = i % 4;
    uchar W = Wp[i]; // gerando um texto plano aleatorio de 1 byte
        
    uchar Y = Wc[i]; // o texto plano encriptado com a chave

    //k1 = w1 XOR y4 XOR y6 XOR y7
    if((bit(Y,4) ^ bit(Y,6) ^ bit(Y,7)) == 1) keypart[K][1]++;
    //k2 = w2 XOR y1 XOR y2 XOR y4 XOR y5 XOR y6 XOR y7
    if((bit(Y,1) ^ bit(Y,2) ^ bit(Y,4) ^ bit(Y,5) ^ bit(Y,6) ^ bit(Y,7)) == 1) keypart[K][2]++;
    //k3 = w3 XOR y1 XOR y3 XOR y5 XOR y6 XOR y8
    if((bit(Y,1) ^ bit(Y,3) ^ bit(Y,5) ^ bit(Y,6) ^ bit(Y,8)) == 1) keypart[K][3]++;
    //k4 = w4 XOR y5 XOR y7
    if((bit(Y,5) ^ bit(Y,7)) == 1) keypart[K][4]++;
    //k5 = w5 XOR y1 XOR y3 XOR y4 XOR y7 XOR y8
    if((bit(Y,1) ^ bit(Y,3) ^ bit(Y,4) ^ bit(Y,7) ^ bit(Y,8)) == 1) keypart[K][5]++;
    //k6 = w6 XOR y3 XOR y6 XOR y7 XOR y8
    if((bit(Y,3) ^ bit(Y,6) ^ bit(Y,7) ^ bit(Y,8)) == 1) keypart[K][6]++;
    //k7 = w7 XOR y3 XOR y5 XOR y6 XOR y7 XOR y8
    if((bit(Y,3) ^ bit(Y,5) ^ bit(Y,6) ^ bit(Y,7) ^ bit(Y,8)) == 1) keypart[K][7]++;
    //k8 = w8 XOR y2 XOR y3 XOR y4 XOR y5 XOR y6 XOR y7 XOR y8
    if((bit(Y,2) ^ bit(Y,3) ^ bit(Y,4) ^ bit(Y,5) ^ bit(Y,6) ^ bit(Y,7) ^ bit(Y,8)) == 1) keypart[K][8]++;
  }
  
  for(int K = 1;K < 5;K++){
    for(int bit = 1;bit < 9;bit++){
        keypart[K][bit] = (float)keypart[K][bit]/ N;
    }
    int value = 0;
    int exp = 0;

    for(int bit = 8;bit > 0;bit--){
        int valueBit = keypart[K][bit] > 0.5 ? 1 : 0;
        value += pow(2,exp) * valueBit;
        printf(" %d ",valueBit);
        exp++;

    }
    printf(" - %d \n",value);
    printf("\n");
  }

  //printf("%f\n",(float )c/(N));

  
  return 0;
  
}
