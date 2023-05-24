#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "../include/idct.h"
#include "../include/jpeg_reader.h"

// Returns two blocks by duplication (see example)
void horizontal_duplication(iM_block* initial,iM_block* left_block, iM_block* right_block){
    /*example: intitial first line {0,1,2,3,4,5,6,7}
                left_block first line {0,0,1,1,2,2,3,3}
                right_bock first line {4,4,5,5,6,6,7,7}*/
    left_block->content = calloc(8,sizeof(uint8_t**));
    right_block->content = calloc(8,sizeof(uint8_t**));
    for (int i = 0; i < 8; i++) {
        left_block->content[i]=calloc(8,sizeof(uint8_t*));
        right_block->content[i]=calloc(8,sizeof(uint8_t*));
        for (int j = 0; j < 8; j++) {
            left_block->content[i][j]=calloc(1,sizeof(uint8_t));
            right_block->content[i][j]=calloc(1,sizeof(uint8_t));
        }
    }
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 4; j++){
            left_block->content[i][2*j]=   initial->content[i][j];
            left_block->content[i][2*j+1]= initial->content[i][j];
            
            right_block->content[i][2*j]=  initial->content[i][4+j];
            right_block->content[i][2*j+1]=initial->content[i][4+j];
        }
    }


}

void vertical_duplication(iM_block* initial,iM_block* upper_block, iM_block* lower_block){
    /*example: intitial first     column {0,1,2,3,4,5,6,7}
                upper_block first column {0,0,1,1,2,2,3,3}
                lower_block first column {4,4,5,5,6,6,7,7}*/
    upper_block->content = calloc(8,sizeof(uint8_t**));
    lower_block->content = calloc(8,sizeof(uint8_t**));
    for (int i = 0; i < 8; i++) {
        upper_block->content[i]=calloc(8,sizeof(uint8_t*));
        lower_block->content[i]=calloc(8,sizeof(uint8_t*));
        for (int j = 0; j < 8; j++) {
            upper_block->content[i][j]=calloc(1,sizeof(uint8_t));
            lower_block->content[i][j]=calloc(1,sizeof(uint8_t));
        }
    }
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 8; j++){
            upper_block->content[2*i][j]=   initial->content[i][j];
            upper_block->content[2*i+1][j]= initial->content[i][j];
            
            lower_block->content[2*i][j]=  initial->content[4+i][j];
            lower_block->content[2*i+1][j]=initial->content[4+i][j];
        }
    }
}
// Returns three blocks by duplication (see example)
void one_to_three_horizontal_duplication(iM_block* initial,iM_block* left_block,iM_block* mid_block ,iM_block* right_block){

    left_block->content = calloc(8,sizeof(uint8_t**));
    mid_block->content = calloc(8,sizeof(uint8_t**));
    right_block->content = calloc(8,sizeof(uint8_t**));
    for (int i = 0; i < 8; i++) {
        left_block->content[i]=calloc(8,sizeof(uint8_t*));
        mid_block->content[i]=calloc(8,sizeof(uint8_t*));
        right_block->content[i]=calloc(8,sizeof(uint8_t*));
        for (int j = 0; j < 8; j++) {
            left_block->content[i][j]=calloc(1,sizeof(uint8_t));
            mid_block->content[i][j]=calloc(1,sizeof(uint8_t));
            right_block->content[i][j]=calloc(1,sizeof(uint8_t));
        }
    }
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 2; j++){
            left_block->content[i][3*j]=   initial->content[i][j];
            left_block->content[i][3*j+1]= initial->content[i][j];
            left_block->content[i][3*j+2]= initial->content[i][j];
            
            mid_block->content[i][3*j+1]=   initial->content[i][j+3];
            mid_block->content[i][3*j+2]= initial->content[i][j+3];
            mid_block->content[i][3*j+3]= initial->content[i][j+3];

            
            right_block->content[i][3*j+2]=  initial->content[i][j+6];
            right_block->content[i][3*j+3]=initial->content[i][j+6];
            right_block->content[i][3*j+4]=initial->content[i][j+6];
        }
        left_block->content[i][6]= initial->content[i][2];
        left_block->content[i][7]= initial->content[i][2];
        mid_block->content[i][0]=  initial->content[i][2];
        mid_block->content[i][7]=  initial->content[i][4];
        right_block->content[i][0]=initial->content[i][4];
        right_block->content[i][1]=initial->content[i][4];
    }


}

void one_to_three_vertical_duplication(iM_block* initial,iM_block* upper_block,iM_block* mid_block ,iM_block* lower_block){

    upper_block->content = calloc(8,sizeof(uint8_t**));
    mid_block->content = calloc(8,sizeof(uint8_t**));
    lower_block->content = calloc(8,sizeof(uint8_t**));
    for (int i = 0; i < 8; i++) {
        upper_block->content[i]=calloc(8,sizeof(uint8_t*));
        mid_block->content[i]=calloc(8,sizeof(uint8_t*));
        lower_block->content[i]=calloc(8,sizeof(uint8_t*));
        for (int j = 0; j < 8; j++) {
            upper_block->content[i][j]=calloc(8,sizeof(uint8_t));
            mid_block->content[i][j]=calloc(8,sizeof(uint8_t));
            lower_block->content[i][j]=calloc(8,sizeof(uint8_t));
        }
    }
    for (int j = 0; j < 8; j++){
        for (int i = 0; i <2; i++){
            upper_block->content[3*i][j]=   initial->content[i][j];
            upper_block->content[3*i+1][j]= initial->content[i][j];
            upper_block->content[3*i+2][j]= initial->content[i][j];
            
            mid_block->content[3*i+1][j]=   initial->content[i+3][j];
            mid_block->content[3*i+2][j]= initial->content[i+3][j];
            mid_block->content[3*i+3][j]= initial->content[i+3][j];
            
            lower_block->content[3*i+2][j]=  initial->content[i+6][j];
            lower_block->content[3*i+3][j]=initial->content[i+6][j];
            lower_block->content[3*i+4][j]=initial->content[i+6][j];
        }
        upper_block->content[6][j]= initial->content[2][j];
        upper_block->content[7][j]= initial->content[2][j];
        mid_block->content[0][j]=  initial ->content[2][j];
        mid_block->content[7][j]=  initial ->content[4][j];
        lower_block->content[0][j]=initial ->content[4][j];
        lower_block->content[1][j]=initial ->content[4][j];
    }


}
// Returns four blocks by duplication
void one_to_four_vertical_duplication(iM_block* initial,iM_block* upper_block,iM_block* upper_mid_block,iM_block* lower_mid_block, iM_block* lower_block){

    upper_block->content = calloc(8,sizeof(uint8_t**));
    upper_mid_block->content = calloc(8,sizeof(uint8_t**));
    lower_mid_block->content = calloc(8,sizeof(uint8_t**));
    lower_block->content = calloc(8,sizeof(uint8_t**));
    for (int i = 0; i < 8; i++) {
        upper_block->content[i]=calloc(8,sizeof(uint8_t*));
        upper_mid_block->content[i] = calloc(8,sizeof(uint8_t*));
        lower_mid_block->content[i] = calloc(8,sizeof(uint8_t*));
        lower_block->content[i]=calloc(8,sizeof(uint8_t*));
        for (int j = 0; j < 8; j++) {
            upper_block->content[i][j]=calloc(1,sizeof(uint8_t));
            upper_mid_block->content[i][j] = calloc(1,sizeof(uint8_t));
            lower_mid_block->content[i][j] = calloc(1,sizeof(uint8_t));
            lower_block->content[i][j]=calloc(1,sizeof(uint8_t));
        }
    }
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 8; j++){

            upper_block->content[4*i][j]=       initial->content[i][j];
            upper_block->content[4*i+1][j]=     initial->content[i][j];
            upper_block->content[4*i+2][j]=     initial->content[i][j];
            upper_block->content[4*i+3][j]=     initial->content[i][j];

            upper_mid_block->content[4*i][j]=   initial->content[2+i][j];
            upper_mid_block->content[4*i+1][j]= initial->content[2+i][j];
            upper_mid_block->content[4*i+2][j]= initial->content[2+i][j];
            upper_mid_block->content[4*i+3][j]= initial->content[2+i][j];

            lower_mid_block->content[4*i][j]=   initial->content[4+i][j];
            lower_mid_block->content[4*i+1][j]= initial->content[4+i][j];
            lower_mid_block->content[4*i+2][j]= initial->content[4+i][j];
            lower_mid_block->content[4*i+3][j]= initial->content[4+i][j];

            lower_block->content[4*i][j]=       initial->content[6+i][j];
            lower_block->content[4*i+1][j]=     initial->content[6+i][j];
            lower_block->content[4*i+2][j]=     initial->content[6+i][j];
            lower_block->content[4*i+3][j]=     initial->content[6+i][j];


        }
    }
}

void one_to_four_horizontal_duplication(iM_block* initial,iM_block* left_block,iM_block* mid_left_block,iM_block* mid_right_block, iM_block* right_block){

    left_block->content = calloc(8,sizeof(uint8_t**));
    mid_left_block->content = calloc(8,sizeof(uint8_t**));
    mid_right_block->content = calloc(8,sizeof(uint8_t**));
    right_block->content = calloc(8,sizeof(uint8_t**));
    for (int i = 0; i < 8; i++) {
        left_block->content[i]=calloc(8,sizeof(uint8_t*));
        mid_left_block->content[i] = calloc(8,sizeof(uint8_t*));
        mid_right_block->content[i] = calloc(8,sizeof(uint8_t*));
        right_block->content[i]=calloc(8,sizeof(uint8_t*));
        for (int j = 0; j < 8; j++) {
            left_block->content[i][j]=calloc(1,sizeof(uint8_t));
            mid_left_block->content[i][j] = calloc(1,sizeof(uint8_t));
            mid_right_block->content[i][j] = calloc(1,sizeof(uint8_t));
            right_block->content[i][j]=calloc(1,sizeof(uint8_t));
        }
    }
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 2; j++){


            left_block->content[i][4*j]=        initial->content[i][j];
            left_block->content[i][4*j+1]=      initial->content[i][j];
            left_block->content[i][4*j+2]=      initial->content[i][j];
            left_block->content[i][4*j+3]=      initial->content[i][j];

            mid_left_block->content[i][4*j]=    initial->content[i][2+j];
            mid_left_block->content[i][4*j+1]=  initial->content[i][2+j];
            mid_left_block->content[i][4*j+2]=  initial->content[i][2+j];
            mid_left_block->content[i][4*j+3]=  initial->content[i][2+j];
            
            mid_right_block->content[i][4*j]=   initial->content[i][4+j];
            mid_right_block->content[i][4*j+1]= initial->content[i][4+j];    
            mid_right_block->content[i][4*j+2]= initial->content[i][4+j];
            mid_right_block->content[i][4*j+3]= initial->content[i][4+j];
            
            right_block->content[i][4*j]=       initial->content[i][6+j];
            right_block->content[i][4*j+1]=     initial->content[i][6+j];    
            right_block->content[i][4*j+2]=     initial->content[i][6+j];
            right_block->content[i][4*j+3]=     initial->content[i][6+j];

        }
    }
}

void modi_LCb(iM_block** LC ,iM_block** LCr,uint8_t h, uint8_t v,uint8_t h0, uint8_t v0 ){
    if (h==1 && v==1){
        if      (h0==1 && v0==1){
            LCr=LC;

        }
        else if ((h0==2 && v0==1)){
            iM_block* left_Cb_block =malloc(sizeof(iM_block));
            iM_block* right_Cb_block=malloc(sizeof(iM_block));

            horizontal_duplication(LC[0],left_Cb_block,right_Cb_block);

            LCr[0]=left_Cb_block;
            LCr[1]=right_Cb_block;
            //free all old


       
        }
        else if ((h0==1 && v0==2)){
            iM_block* upper_Cb_block =malloc(sizeof(iM_block));
            iM_block* lower_Cb_block=malloc(sizeof(iM_block));

            vertical_duplication(LC[0],upper_Cb_block,lower_Cb_block);


            LCr[0]=upper_Cb_block;
            LCr[1]=lower_Cb_block;

            //free all old

       
        }
        else if ((h0==1 && v0==4)){
            iM_block* upper_Cb_Block=malloc(sizeof(iM_block));
            iM_block* upper_mid_Cb_Block=malloc(sizeof(iM_block));
            iM_block* lower_mid_Cb_Block=malloc(sizeof(iM_block));
            iM_block* lower_Cb_Block=malloc(sizeof(iM_block));


            one_to_four_vertical_duplication(LC[0],upper_Cb_Block, upper_mid_Cb_Block, lower_mid_Cb_Block,  lower_Cb_Block);

            LCr[0]=upper_Cb_Block;
            LCr[1]=upper_mid_Cb_Block;
            LCr[2]=lower_mid_Cb_Block;
            LCr[3]=lower_Cb_Block;

            //free all old
        }
        else if ((h0==4 && v0==1)){
            iM_block* left_Cb_Block=malloc(sizeof(iM_block));
            iM_block* mid_left_Cb_Block=malloc(sizeof(iM_block));
            iM_block* mid_right_Cb_Block=malloc(sizeof(iM_block));
            iM_block* right_Cb_Block=malloc(sizeof(iM_block));

            one_to_four_horizontal_duplication( LC[0], left_Cb_Block,mid_left_Cb_Block, mid_right_Cb_Block,  right_Cb_Block);

            LCr[0]=left_Cb_Block;
            LCr[1]=mid_left_Cb_Block;
            LCr[2]=mid_right_Cb_Block;
            LCr[3]=right_Cb_Block;

        }
        else if ((h0==2 && v0==2)){
            iM_block* upper_left_Cb_block =malloc(sizeof(iM_block));
            iM_block* upper_right_Cb_block=malloc(sizeof(iM_block));
            iM_block* lower_left_Cb_block =malloc(sizeof(iM_block));
            iM_block* lower_right_Cb_block=malloc(sizeof(iM_block));  


            
            //temporary
            iM_block* left_Cb_block =malloc(sizeof(iM_block));
            iM_block* right_Cb_block=malloc(sizeof(iM_block)); 



            //one hori then two verti
            horizontal_duplication(LC[0],left_Cb_block,right_Cb_block);

            vertical_duplication(left_Cb_block, upper_left_Cb_block ,lower_left_Cb_block);
            vertical_duplication(right_Cb_block,upper_right_Cb_block,lower_right_Cb_block);
            
            LCr[0]=upper_left_Cb_block;
            LCr[1]=upper_right_Cb_block;
            LCr[2]=lower_left_Cb_block;
            LCr[3]=lower_right_Cb_block;   


            free(left_Cb_block);
            free(right_Cb_block); 

            free(LC);
             //il faut free l'interieur aussi
        }
        else if ((h0==2 && v0==4)){
            //blocs are ordered this way
            /*
            0 1
            2 3
            4 5
            6 7
            */
            iM_block* Cb_0=malloc(sizeof(iM_block)); 
            iM_block* Cb_1=malloc(sizeof(iM_block));
            iM_block* Cb_2=malloc(sizeof(iM_block));
            iM_block* Cb_3=malloc(sizeof(iM_block));
            iM_block* Cb_4=malloc(sizeof(iM_block));
            iM_block* Cb_5=malloc(sizeof(iM_block));
            iM_block* Cb_6=malloc(sizeof(iM_block));
            iM_block* Cb_7=malloc(sizeof(iM_block));

            //temporary
            iM_block* left_Cb_block =malloc(sizeof(iM_block));
            iM_block* right_Cb_block=malloc(sizeof(iM_block)); 

            horizontal_duplication(LC[0],left_Cb_block,right_Cb_block);

            one_to_four_vertical_duplication(left_Cb_block , Cb_0 ,Cb_2,Cb_4,Cb_6);
            one_to_four_vertical_duplication(right_Cb_block, Cb_1 ,Cb_3,Cb_5,Cb_7);

            LCr[0]=Cb_0;
            LCr[1]=Cb_1;
            LCr[2]=Cb_2;
            LCr[3]=Cb_3;
            LCr[4]=Cb_4;
            LCr[5]=Cb_5;
            LCr[6]=Cb_6;
            LCr[7]=Cb_7;

            free(left_Cb_block);
            free(right_Cb_block); 


            free(LC); //il faut free l'interieur aussi
    

        }
        else if ((h0==4 && v0==2)){
            //blocs are ordered this way
            /*
            0 1 2 3
            4 5 6 7             
            */

            iM_block* Cb_0=malloc(sizeof(iM_block)); 
            iM_block* Cb_1=malloc(sizeof(iM_block));
            iM_block* Cb_2=malloc(sizeof(iM_block));
            iM_block* Cb_3=malloc(sizeof(iM_block));
            iM_block* Cb_4=malloc(sizeof(iM_block));
            iM_block* Cb_5=malloc(sizeof(iM_block));
            iM_block* Cb_6=malloc(sizeof(iM_block));
            iM_block* Cb_7=malloc(sizeof(iM_block));

            //temporary
            iM_block* upper_Cb_block =malloc(sizeof(iM_block));
            iM_block* lower_Cb_block=malloc(sizeof(iM_block)); 


            vertical_duplication(LC[0],upper_Cb_block,lower_Cb_block);

            one_to_four_horizontal_duplication(upper_Cb_block, Cb_0 ,Cb_1,Cb_2,Cb_3);
            one_to_four_horizontal_duplication(lower_Cb_block, Cb_4 ,Cb_5,Cb_6,Cb_7);

            LCr[0]=Cb_0;
            LCr[1]=Cb_1;
            LCr[2]=Cb_2;
            LCr[3]=Cb_3;
            LCr[4]=Cb_4;
            LCr[5]=Cb_5;
            LCr[6]=Cb_6;
            LCr[7]=Cb_7;

            free(upper_Cb_block);
            free(lower_Cb_block);

            free(LC);
            //il faut free l'interieur aussi
    
        }
        else if ((h0==3 && v0==1)){
            iM_block* left_Cb_block =malloc(sizeof(iM_block));
            iM_block* mid_Cb_block =malloc(sizeof(iM_block));
            iM_block* right_Cb_block=malloc(sizeof(iM_block));
            one_to_three_horizontal_duplication(LC[0],left_Cb_block,mid_Cb_block,right_Cb_block);

            LCr[0]=left_Cb_block;
            LCr[1]=mid_Cb_block;
            LCr[2]=right_Cb_block;
            //free all old

        }
        else if ((h0==1 && v0==3)){
            iM_block* upper_Cb_block =malloc(sizeof(iM_block));
            iM_block* mid_Cb_block =malloc(sizeof(iM_block));
            iM_block* lower_Cb_block=malloc(sizeof(iM_block));

            one_to_three_vertical_duplication(LC[0],upper_Cb_block,mid_Cb_block,lower_Cb_block);

            LCr[0]=upper_Cb_block;
            LCr[1]=mid_Cb_block;
            LCr[2]=lower_Cb_block;

            //free all old

        }
        else if ((h0==2 && v0==3)){
            //blocs are ordered this way
            /*
            0 1
            2 3
            4 5
            
            */
            iM_block* Cb_0=malloc(sizeof(iM_block)); 
            iM_block* Cb_1=malloc(sizeof(iM_block));
            iM_block* Cb_2=malloc(sizeof(iM_block));
            iM_block* Cb_3=malloc(sizeof(iM_block));
            iM_block* Cb_4=malloc(sizeof(iM_block));
            iM_block* Cb_5=malloc(sizeof(iM_block));


            //temporary
            iM_block* left_Cb_block =malloc(sizeof(iM_block));
            iM_block* right_Cb_block=malloc(sizeof(iM_block)); 

            horizontal_duplication(LC[0],left_Cb_block,right_Cb_block);

            one_to_three_vertical_duplication(left_Cb_block , Cb_0 ,Cb_2,Cb_4);
            one_to_three_vertical_duplication(right_Cb_block, Cb_1 ,Cb_3,Cb_5);

            LCr[0]=Cb_0;
            LCr[1]=Cb_1;
            LCr[2]=Cb_2;
            LCr[3]=Cb_3;
            LCr[4]=Cb_4;
            LCr[5]=Cb_5;


            free(left_Cb_block);
            free(right_Cb_block); 


            free(LC); //il faut free l'interieur aussi
        }
        else if ((h0==3 && v0==2)){
            //blocs are ordered this way
            /*
            0 1 2 
            3 4 5              
            */

            iM_block* Cb_0=malloc(sizeof(iM_block)); 
            iM_block* Cb_1=malloc(sizeof(iM_block));
            iM_block* Cb_2=malloc(sizeof(iM_block));
            iM_block* Cb_3=malloc(sizeof(iM_block));
            iM_block* Cb_4=malloc(sizeof(iM_block));
            iM_block* Cb_5=malloc(sizeof(iM_block));


            //temporary
            iM_block* upper_Cb_block =malloc(sizeof(iM_block));
            iM_block* lower_Cb_block=malloc(sizeof(iM_block)); 


            vertical_duplication(LC[0],upper_Cb_block,lower_Cb_block);

            one_to_three_horizontal_duplication(upper_Cb_block, Cb_0 ,Cb_1,Cb_2);
            one_to_three_horizontal_duplication(lower_Cb_block, Cb_3 ,Cb_4,Cb_5);

            LCr[0]=Cb_0;
            LCr[1]=Cb_1;
            LCr[2]=Cb_2;
            LCr[3]=Cb_3;
            LCr[4]=Cb_4;
            LCr[5]=Cb_5;


            free(upper_Cb_block);
            free(lower_Cb_block);

            free(LC);
            //il faut free l'interieur aussi
        }
        else{
            printf("CASE NOT TREATED type 0:");       
        }
    }
    else if(h==2 && v==1){
        if      (h0==2 && v0==1){
            LCr=LC;
        }
        else if(h0==2 && v0==2){
            iM_block* upper_left_Cb_block =malloc(sizeof(iM_block));
            iM_block* upper_right_Cb_block=malloc(sizeof(iM_block));


            iM_block* lower_left_Cb_block =malloc(sizeof(iM_block));
            iM_block* lower_right_Cb_block=malloc(sizeof(iM_block));


            vertical_duplication(LC[0],upper_left_Cb_block,lower_left_Cb_block);
            vertical_duplication(LC[1],upper_right_Cb_block,lower_right_Cb_block);

            LCr[0]=upper_left_Cb_block;
            LCr[1]=upper_right_Cb_block;
            LCr[2]=lower_left_Cb_block;
            LCr[3]=lower_right_Cb_block;   


            free(LC);//il faut free l'interieur aussi
        }
        else if((h0==4 && v0==1)){
            iM_block* left_Cb_Block=malloc(sizeof(iM_block));
            iM_block* mid_left_Cb_Block=malloc(sizeof(iM_block));
            iM_block* mid_right_Cb_Block=malloc(sizeof(iM_block));
            iM_block* right_Cb_Block=malloc(sizeof(iM_block));

            horizontal_duplication(LC[0], left_Cb_Block,mid_left_Cb_Block );
            horizontal_duplication(LC[1], mid_right_Cb_Block,  right_Cb_Block);


            LCr[0]=left_Cb_Block;
            LCr[1]=mid_left_Cb_Block;
            LCr[2]=mid_right_Cb_Block;
            LCr[3]=right_Cb_Block;

        }        
        else if ((h0==2 && v0==3)){
            //blocs are ordered this way
            /*
            0 1
            2 3
            4 5
            
            */
            iM_block* Cb_0=malloc(sizeof(iM_block)); 
            iM_block* Cb_1=malloc(sizeof(iM_block));
            iM_block* Cb_2=malloc(sizeof(iM_block));
            iM_block* Cb_3=malloc(sizeof(iM_block));
            iM_block* Cb_4=malloc(sizeof(iM_block));
            iM_block* Cb_5=malloc(sizeof(iM_block));




            one_to_three_vertical_duplication(LC[0] , Cb_0 ,Cb_2,Cb_4);
            one_to_three_vertical_duplication(LC[1], Cb_1 ,Cb_3,Cb_5);

            LCr[0]=Cb_0;
            LCr[1]=Cb_1;
            LCr[2]=Cb_2;
            LCr[3]=Cb_3;
            LCr[4]=Cb_4;
            LCr[5]=Cb_5;


            free(LC); //il faut free l'interieur aussi
        }

        else{
            printf("CASE NOT TREATED type 1:");
        }
    }
    
    else if(h==1 && v==2){
        if (h0==1 && v0==2){
            LCr=LC;
            }
        else if(h0==2 && v0==2){
            iM_block* upper_left_Cb_block =malloc(sizeof(iM_block));
            iM_block* upper_right_Cb_block=malloc(sizeof(iM_block));

            iM_block* lower_left_Cb_block =malloc(sizeof(iM_block));
            iM_block* lower_right_Cb_block=malloc(sizeof(iM_block));


            horizontal_duplication(LC[0],upper_left_Cb_block,upper_right_Cb_block);
            horizontal_duplication(LC[1],lower_left_Cb_block,lower_right_Cb_block);
            
            LCr[0]=upper_left_Cb_block;
            LCr[1]=upper_right_Cb_block;
            LCr[2]=lower_left_Cb_block;
            LCr[3]=lower_right_Cb_block;   


            free(LC); //il faut free l'interieur aussi
        }
        else if((h0==1 && v0==4)){
            iM_block* upper_Cb_Block=malloc(sizeof(iM_block));
            iM_block* upper_mid_Cb_Block=malloc(sizeof(iM_block));
            iM_block* lower_mid_Cb_Block=malloc(sizeof(iM_block));
            iM_block* lower_Cb_Block=malloc(sizeof(iM_block));

            vertical_duplication(LC[0],upper_Cb_Block,      upper_mid_Cb_Block);
            vertical_duplication(LC[1],lower_mid_Cb_Block,  lower_Cb_Block);

            LCr[0]=upper_Cb_Block;
            LCr[1]=upper_mid_Cb_Block;
            LCr[2]=lower_mid_Cb_Block;
            LCr[3]=lower_Cb_Block;
            free(LC);
        }
        else if ((h0==3 && v0==2)){
            //blocs are ordered this way
            /*
            0 1 2 
            3 4 5              
            */

            iM_block* Cb_0=malloc(sizeof(iM_block)); 
            iM_block* Cb_1=malloc(sizeof(iM_block));
            iM_block* Cb_2=malloc(sizeof(iM_block));
            iM_block* Cb_3=malloc(sizeof(iM_block));
            iM_block* Cb_4=malloc(sizeof(iM_block));
            iM_block* Cb_5=malloc(sizeof(iM_block));

            one_to_three_horizontal_duplication(LC[0], Cb_0 ,Cb_1,Cb_2);
            one_to_three_horizontal_duplication(LC[1], Cb_3 ,Cb_4,Cb_5);

            LCr[0]=Cb_0;
            LCr[1]=Cb_1;
            LCr[2]=Cb_2;
            LCr[3]=Cb_3;
            LCr[4]=Cb_4;
            LCr[5]=Cb_5;

            free(LC);
            //il faut free l'interieur aussi
        }

        else{
            printf("CASE NOT TREATED type 2:");
        }
    
    }
 
    else if(((h==3 && v==1))){
        if  (h0==3 && v0==1){
            LCr=LC;
        }
        else if ((h0==3 && v0==2)){
            //blocs are ordered this way
            /*
            0 1 2 
            3 4 5              
            */

            iM_block* Cb_0=malloc(sizeof(iM_block)); 
            iM_block* Cb_1=malloc(sizeof(iM_block));
            iM_block* Cb_2=malloc(sizeof(iM_block));
            iM_block* Cb_3=malloc(sizeof(iM_block));
            iM_block* Cb_4=malloc(sizeof(iM_block));
            iM_block* Cb_5=malloc(sizeof(iM_block));

            vertical_duplication(LC[0],Cb_0 ,Cb_3);
            vertical_duplication(LC[1],Cb_1 ,Cb_4);
            vertical_duplication(LC[2],Cb_2 ,Cb_5);

            LCr[0]=Cb_0;
            LCr[1]=Cb_1;
            LCr[2]=Cb_2;
            LCr[3]=Cb_3;
            LCr[4]=Cb_4;
            LCr[5]=Cb_5;

            free(LC);
            //il faut free l'interieur aussi
        }
        else{
            printf("CASE NOT TREATED type 3:");
        }
    }

    else if(((h==1 && v==3))){
        if  (h0==1 && v0==3){
            LCr=LC;
        }
        else if ((h0==2 && v0==3)){
            //blocs are ordered this way
            /*
            0 1
            2 3
            4 5
            
            */
            iM_block* Cb_0=malloc(sizeof(iM_block)); 
            iM_block* Cb_1=malloc(sizeof(iM_block));
            iM_block* Cb_2=malloc(sizeof(iM_block));
            iM_block* Cb_3=malloc(sizeof(iM_block));
            iM_block* Cb_4=malloc(sizeof(iM_block));
            iM_block* Cb_5=malloc(sizeof(iM_block));

            horizontal_duplication(LC[0],Cb_0,Cb_1);
            horizontal_duplication(LC[1],Cb_2,Cb_3);
            horizontal_duplication(LC[2],Cb_4,Cb_5);

            LCr[0]=Cb_0;
            LCr[1]=Cb_1;
            LCr[2]=Cb_2;
            LCr[3]=Cb_3;
            LCr[4]=Cb_4;
            LCr[5]=Cb_5;



            free(LC); //il faut free l'interieur aussi
        }

        else{
            printf("CASE NOT TREATED type 4:");
        }
    }
    
    else{
        printf("CASE NOT TREATED type 5:");
    }
}
// gives the upsampled iM_MCU given the intitial one
void modi_mcu_r(iM_MCU* old_im_mcu, iM_MCU* up_samp_im_mcu,uint8_t* S_fact){
    up_samp_im_mcu->LY=old_im_mcu->LY;
    
    if((S_fact[0]!=S_fact[2]) || (S_fact[1]!=S_fact[3])){
        
        up_samp_im_mcu->LCb= calloc(S_fact[0]*S_fact[1],sizeof(iM_block*));
        modi_LCb(old_im_mcu->LCb,up_samp_im_mcu->LCb ,S_fact[2], S_fact[3],S_fact[0], S_fact[1] );
    }
    else{
        up_samp_im_mcu->LCb=old_im_mcu->LCb;
    }
    if((S_fact[0]!=S_fact[4]) || (S_fact[1]!=S_fact[5])){
        
        up_samp_im_mcu->LCr= calloc(S_fact[0]*S_fact[1],sizeof(iM_block*));
        modi_LCb(old_im_mcu->LCr,up_samp_im_mcu->LCr ,S_fact[4], S_fact[5],S_fact[0], S_fact[1] );;
    }
    else{
        up_samp_im_mcu->LCr=old_im_mcu->LCr;
    }

}

//this puts the up_sampled version of im_lmcu in result and returns it (adding iM_MCUs and all the steps) 
iM_LMCU* up_sample(iM_LMCU* im_lmcu,struct SOF* sof){
    iM_LMCU* result;
    uint8_t* S_fact;
    if (sof->components_number==3){
        result=malloc(sizeof(iM_LMCU));
        result->iM_MCUs=calloc(im_lmcu->MCU_counter,sizeof(iM_MCU*));
        result->MCU_counter=im_lmcu->MCU_counter;
        S_fact=malloc(2*(sof->components_number)*sizeof(uint8_t));

        for (uint8_t i=0;i<3;i++){
            S_fact[2*i]=sof->sampling_horizontal[i];
            S_fact[2*i+1]=sof->sampling_vertical[i];
        }
    
    
    for (uint32_t i=0;i<im_lmcu->MCU_counter;i++){
        result->iM_MCUs[i]=malloc(sizeof(iM_MCU));
        
        modi_mcu_r(im_lmcu->iM_MCUs[i] ,result->iM_MCUs[i],S_fact);
        //free(im_lmcu->iM_MCUs[i]);//il faut  pas tjrs free LCb/LCr et pas leurs blocs mais pas LY
    }   

    }
    free(S_fact);
    return result;
}



