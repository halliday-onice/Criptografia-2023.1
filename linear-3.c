#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define uchar unsigned char 
#define SBOXLEN 16
#define INPUTBYTES 4
//uchar SBOX[256] = {129, 249, 58, 80, 242, 68, 86, 195, 60, 154, 152, 37, 189, 150, 219, 160, 231, 131, 218, 50, 6, 217, 36, 125, 252, 81, 147, 153, 119, 185, 122, 57, 42, 52, 173, 87, 111, 182, 8, 84, 10, 193, 194, 228, 89, 12, 105, 198, 40, 25, 14, 223, 156, 126, 45, 43, 18, 190, 133, 102, 229, 158, 132, 72, 5, 235, 29, 108, 65, 75, 239, 179, 222, 248, 210, 196, 151, 141, 103, 3, 78, 180, 115, 28, 41, 24, 53, 11, 82, 0, 134, 20, 77, 83, 94, 214, 117, 255, 148, 48, 85, 234, 159, 51, 136, 215, 71, 174, 175, 33, 227, 35, 224, 7, 253, 113, 145, 99, 233, 161, 164, 98, 92, 23, 32, 155, 205, 101, 221, 106, 59, 64, 116, 62, 144, 139, 178, 247, 241, 220, 142, 211, 34, 67, 27, 168, 120, 188, 21, 138, 110, 88, 69, 4, 232, 170, 56, 44, 244, 124, 73, 90, 30, 250, 46, 123, 165, 49, 183, 209, 76, 47, 163, 226, 246, 55, 177, 140, 207, 184, 208, 191, 187, 22, 118, 143, 236, 146, 167, 2, 197, 245, 112, 181, 166, 162, 104, 192, 26, 172, 38, 186, 206, 251, 61, 216, 93, 9, 240, 16, 19, 203, 127, 238, 95, 15, 200, 63, 91, 213, 254, 31, 17, 230, 157, 225, 201, 130, 1, 149, 96, 70, 100, 97, 121, 66, 137, 212, 171, 74, 176, 202, 204, 237, 54, 114, 13, 128, 243, 79, 109, 39, 135, 169, 199, 107};
uchar SBOX[16] = { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 };

uchar bit( uchar x, uchar i ) {
    return (x >> (4-i)) & 1;
}


uchar E( uchar w, uchar k ) {
    uchar x = w ^ k;
    uchar y = SBOX[x];
    return y;
}

void genBiasTable() {
    uchar bias[SBOXLEN][SBOXLEN]; //gonna save the bias information here
    
    int counter = 0;
    

    int alpha = 0;
    for( uchar x = 0; x < SBOXLEN; x++ ) {
        printf("c|");
    }
    printf("\n & ");
    for( uchar x = 1; x < SBOXLEN; x++ ) {
        if( x != SBOXLEN - 1 ) {
            printf(" {\\bf %.1X} &", x);
        } else {
            printf(" {\\bf %.1X} \\\\\\hline\n", x);
        }
    }
    
    for( uchar x = 1; x < SBOXLEN; x++ ) {
        printf(" {\\bf %.1X} &", x);
        for( uchar beta = 1; beta < SBOXLEN; beta++ ) {
            
            alpha = 0;
            for( uchar X = 0; X < SBOXLEN; X++ ) {
                int a;
                if((a=((bit(X, 1)&bit(x, 1)) ^ 
                    (bit(X, 2)&bit(x, 2)) ^
                    (bit(X, 3)&bit(x, 3)) ^
                    (bit(X, 4)&bit(x, 4)) ^
                    (bit(SBOX[X], 1)&bit(beta, 1)) ^
                    (bit(SBOX[X], 2)&bit(beta, 2)) ^
                    (bit(SBOX[X], 3)&bit(beta, 3)) ^
                    (bit(SBOX[X], 4)&bit(beta, 4)))) == 0 )       
                        alpha++;
            }
            bias[x][beta] =  alpha - 8;
            //printf(">=>=>= %d\n", alpha - 8);       
            if( beta != SBOXLEN - 1) {
                printf("%d & ", bias[x][beta]); // 2ˆ4 - 1
            } else {
                printf("%d\\\\\\hline\n", bias[x][beta]);
            }        
        }
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
  /*
  srand( time(NULL) );
  uchar K = 11;
  int c = 0;
  int N = 10;
  for( int i = 0; i < N; i++ ) {
      //for( uchar K = 7; K < 8; K++ ) {
          uchar W = rand() & 0x100; 
          uchar W = 'testando um dois tres';
          uchar Y = E(W,K);
          if( bit(Y,2) ^ bit(Y,3) ^ bit(Y,4) ^ bit(W,4) == 1 ) {
             c++;
          }        
      //}
  } 
  printf("%f\n",(float )c/(N));
  */
  return 0;
  
}
