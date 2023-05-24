#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "../include/izz.h"
#include "../include/idct.h"

#define M_PI 3.14159265358979323846


// 4 stages reversed :

// DEFINE BUILDING BLOCKS OPERATIONS 

void crossed(double* I0,double* I1, double o0, double o1){
    
    // this function does the inverse of stage 4

    *I0=(o1+o0)/2;
    *I1=(o0-o1)/2;
    
}

void inverse_rotation(double* I0,double* I1,double o0,double o1,double k,uint16_t n){
   
    // this function calculates the inverse rotation

    *I0=(1/k)*((o0*cos((n*M_PI)/16)-o1*sin((n*M_PI)/16)));
    *I1=(1/k)*((o1*cos((n*M_PI)/16)+o0*sin((n*M_PI)/16)));
}

void inverse_multiplication(double* I,double O){

    *I=(1/sqrt(2))*O;
}

void switch_vect_I(double*** vect,double*** next_I__post4){
    double **temp=*vect;

    *vect=*next_I__post4;
    *next_I__post4=temp;
}

void fast_idct_vector(double** vector){

// this function does the inverse dct on a vector block ( 1D )
for (uint8_t i=0;i<8;i++)
        *vector[i]*=sqrt(8);

double* next_I_post4=malloc(8*sizeof(double));
    
    //Stage 4 inversed 


        crossed(&next_I_post4[7],&next_I_post4[4],*(vector)[1],*(vector)[7]);
        inverse_multiplication(&next_I_post4[5],*(vector[3]));
        inverse_multiplication(&next_I_post4[6],*(vector[5]));
        next_I_post4[0]=*(vector[0]);
        next_I_post4[1]=*(vector[4]);
        next_I_post4[2]=*(vector[2]);
        next_I_post4[3]=*(vector[6]);


    double* next_I_post3=malloc(8*sizeof(double));
   


    //Stage 3 inversed
        crossed(&next_I_post3[0],&next_I_post3[1],next_I_post4[0],next_I_post4[1]);
        crossed(&next_I_post3[4],&next_I_post3[6],next_I_post4[4],next_I_post4[6]);
        crossed(&next_I_post3[7],&next_I_post3[5],next_I_post4[7],next_I_post4[5]);
        inverse_rotation(&next_I_post3[2],&next_I_post3[3],next_I_post4[2],next_I_post4[3],sqrt(2),6);

free(next_I_post4);

double* next_I_post2=malloc(8*sizeof(double));
    //Stage 2 inversed
        crossed(&next_I_post2[0],&next_I_post2[3],next_I_post3[0],next_I_post3[3]);
        crossed(&next_I_post2[1],&next_I_post2[2],next_I_post3[1],next_I_post3[2]);
        inverse_rotation(&next_I_post2[4],&next_I_post2[7],next_I_post3[4],next_I_post3[7],1,3);
        inverse_rotation(&next_I_post2[5],&next_I_post2[6],next_I_post3[5],next_I_post3[6],1,1);

free(next_I_post3);

    //Stage 1 inversed
        crossed(vector[0],vector[7],next_I_post2[0],next_I_post2[7]);
        crossed(vector[1],vector[6],next_I_post2[1],next_I_post2[6]);
        crossed(vector[2],vector[5],next_I_post2[2],next_I_post2[5]);
        crossed(vector[3],vector[4],next_I_post2[3],next_I_post2[4]);

free(next_I_post2);
}

void transpose_table(double ***table)
{
    double temp1, temp2;
    for (uint8_t i = 1; i < 8; i++) {
        for (uint8_t j = 0; j < i; j++) {
            temp1 = *table[i][j];
            temp2 = *table[j][i];
            *table[i][j] = temp2;
            *table[j][i] = temp1;
        }
    }
}

void fast_idct(int16_t*** izz_matrix,uint8_t*** out_matrix){
    
    double*** matrix_izz = malloc(8*sizeof(double**));
    for (int i = 0; i < 8; i++) {
        matrix_izz[i]=malloc(8*sizeof(double*));
        for (int j = 0; j < 8; j++) {
            matrix_izz[i][j]=malloc(sizeof(double));
        }
    }
    for (uint8_t i=0;i<8;i++){
        for (uint8_t j=0;j<8;j++){
            *matrix_izz[i][j]=(double)*izz_matrix[i][j];
        }
    }

    for (uint8_t i=0;i<8;i++){
        fast_idct_vector(matrix_izz[i]);
    }

    transpose_table(matrix_izz);
    
    for (uint8_t i=0;i<8;i++){
        fast_idct_vector(matrix_izz[i]);
    }
    transpose_table(matrix_izz);

    for (uint8_t i=0;i<8;i++){
        for (uint8_t j=0;j<8;j++){
            *matrix_izz[i][j]+=128;
            *matrix_izz[i][j]=*matrix_izz[i][j] <0 ? 0: *matrix_izz[i][j];
            *matrix_izz[i][j]=*matrix_izz[i][j] >255 ? 255: *matrix_izz[i][j];

            *(out_matrix[i][j])=(uint8_t) round(*(matrix_izz[i][j]));
            

        }
    }
    for (uint8_t i=0;i<8;i++){
        for (uint8_t j=0;j<8;j++){
            free(matrix_izz[i][j]);
        }
        free(matrix_izz[i]);
    }
    free(matrix_izz);

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
    fast_idct(M_block->content,new_iM_Block->content);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
        free(M_block->content[i][j]); ////!!!!! careful
        }
        free(M_block->content[i]);
    }
    free(M_block->content);
    free(M_block);
}


