void genBiasTable2() {
    int c = 0;
    for( uchar x = 0; x < 16; x++ ) {
        printf("c|");
    }
    printf("\n & ");
    for( uchar x = 1; x < 16; x++ ) {
        if( x != 15 ) {
            printf(" {\\bf %.1X} &", x);
        } else {
            printf(" {\\bf %.1X} \\\\\\hline\n", x);
        }
    }
    
    for( uchar x = 1; x < 16; x++ ) {
        printf(" {\\bf %.1X} &", x);
        for( uchar y = 1; y < 16; y++ ) {
            
            c = 0;
            for( uchar X = 0; X < 16; X++ ) {
                int a;
                if((a=((bit(X, 1)&bit(x, 1)) ^ 
                    (bit(X, 2)&bit(x, 2)) ^
                    (bit(X, 3)&bit(x, 3)) ^
                    (bit(X, 4)&bit(x, 4)) ^
                    (bit(SBOX[X], 1)&bit(y, 1)) ^
                    (bit(SBOX[X], 2)&bit(y, 2)) ^
                    (bit(SBOX[X], 3)&bit(y, 3)) ^
                    (bit(SBOX[X], 4)&bit(y, 4)))) == 0 )       
                        c++;
            }        
            if( y != 15 ) {
                printf("%d & ", c-8 );
            } else {
                printf("%d\\\\\\hline\n", c-8);
            }        
        }
    }
}