/*#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "../include/izz.h"
#include "../include/idct.h"


#define PI 3.14159265358979323846

double C(uint8_t x)
{
        double value = 1;

        if (x == 0)
                value = 1/sqrt(2);

        return value;
}

uint8_t S(uint8_t x,uint8_t y,int16_t*** tab_freq){
    /* TAB FREQ IS THE 8*8 BLOCK AFTER IZZ AND IQ *//*
    double sum=0;
    
    for (uint8_t i=0;i<8;i++){
        for (uint8_t j=0;j<8;j++){
            sum+=C(i)*C(j)*cos((2*x+1)*i*PI/16)*cos((2*y+1)*j*PI/16)* (double)*(tab_freq)[i][j];
                
        }
    }
    
    double sum_total=sum/4+128.;
    if (sum_total<0) 
        return 0;
    else if (sum_total>255) 
        return (uint8_t) 255;
    else 
    return (uint8_t) floor(sum_total+0.5);
    }


void idct(uint8_t*** out,int16_t*** tab_freq){
    for (int i=0;i<8;i++){
        for (int j=0;j<8;j++){
            uint8_t value=S(i,j,tab_freq);
            *(out)[i][j]=value;
        }
    }
}

void create_iM_block(iM_block* new_iM_Block, M_block*  M_block){
    
    new_iM_Block->block_type = M_block->block_type;
    new_iM_Block->content = calloc(8,sizeof(uint8_t**));

    for (int i = 0; i < 8; i++) {
        new_iM_Block->content[i]=calloc(8,sizeof(uint8_t*));
        for (int j = 0; j < 8; j++) {
            new_iM_Block->content[i][j]=calloc(8,sizeof(uint8_t));
        }
    }
    idct(new_iM_Block->content,M_block->content);
    free(M_block->content);
    free(M_block);
}



/*int main(void){
    int16_t freq[8][8]={{6,-9,-1,-1,0,0,0,0},{4,1,-1,1,1,0,0,1},{-7,0,0,-1,0,0,0,0},{2,1,-1,0,0,0,0,-1},{-1,0,0,0,0,0,0,0},{1,0,-1,0,0,0,0,0},{0,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0}};
    uint8_t out[8][8];
    printf("entier: %i\n",freq[0][1]);
    idct(out,freq);
    for (int i=0;i<8;i++){
        for (int j=0;j<8;j++){
            printf("out[%i,%i]= %02x\n",i,j,out[i][j]);
        }
    }
    
}*/