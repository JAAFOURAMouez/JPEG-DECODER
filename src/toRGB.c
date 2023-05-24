#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "../include/idct.h"

uint8_t SATURATED(float val){
    if (val < 0){ return 0; }
    if (val > 255){ return 255;}
    else return  floor(val);
}

void one_YCbCr_mcu_to_rgb(iM_MCU* iM_MCU,uint8_t** mcu_rgb,uint32_t nb_horz_blocks_in_mcu,uint32_t nb_vertical_blocks_in_mcu){
    
    uint32_t nb_blocks=nb_horz_blocks_in_mcu*nb_vertical_blocks_in_mcu;
    float R,G,B;
    for (uint32_t i=0;i<nb_blocks;i++){
        for (uint32_t j=0;j<8;j++){
            for (uint32_t k=0;k<8;k++){
                R = *(iM_MCU->LY[i]->content)[j][k] - 0.0009267 * (*(iM_MCU->LCb[i]->content)[j][k] - 128) + 1.4016868 * (*(iM_MCU->LCr[i]->content)[j][k] - 128);
                G = *(iM_MCU->LY[i]->content)[j][k] - 0.3436954 * (*(iM_MCU->LCb[i]->content)[j][k] - 128) - 0.7141690 * (*(iM_MCU->LCr[i]->content)[j][k] - 128);
                B = *(iM_MCU->LY[i]->content)[j][k] + 1.7721604 * (*(iM_MCU->LCb[i]->content)[j][k] - 128) + 0.0009902 * (*(iM_MCU->LCr[i]->content)[j][k] - 128);
            
                mcu_rgb[i*64 + j*8 + k][0]=SATURATED(R) ;
                mcu_rgb[i*64 + j*8 + k][1]=SATURATED(G) ; 
                mcu_rgb[i*64 + j*8 + k][2]=SATURATED(B) ;
    
            }
        }
    }

}

void one_Y_mcu_to_rgb(iM_MCU* IMCU_Y,uint8_t* mcu_rgb,uint32_t nb_horz_blocks_in_mcu,uint32_t nb_vertical_blocks_in_mcu){
        
        uint32_t nb_blocks = nb_horz_blocks_in_mcu * nb_vertical_blocks_in_mcu;
        uint8_t g;
        
        // Convert MCUs to RGB using Y MCUs 
        
        for (uint32_t i = 0; i < nb_blocks; i++) {
            for (uint32_t j=0;j<8;j++){
                for (uint32_t k=0;k<8;k++){
                    g = *(IMCU_Y->LY[i]->content)[j][k];
                    mcu_rgb[i*64 + j*8 + k] = g << 16 | g << 8 | g;
        
        }
}
}
}

/*int main(void){
    uint8_t** mcu_rgb=malloc(128*sizeof(uint8_t*));
    uint8_t** mcu_rgb2=malloc(128*sizeof(uint8_t*));
    for (int i=0;i<128;i++){
        mcu_rgb[i]=malloc(3*sizeof(uint8_t));
        mcu_rgb2[i]=malloc(3*sizeof(uint8_t));

    }
    one_YCbCr_mcu_to_rgb(mcu_rgb,mcu_rgb2,2,1);
    for (int i=0;i<128;i++){
        printf("{");
        for (int j=0;j<2;j++){
        printf("0x%02x,",mcu_rgb[i][j]);}
        printf("0x%02x",mcu_rgb[i][2]);
        printf("},\n");
    
        
    }
    printf("OKKKKKKKKKKKKKKK\n");

    for (int i=0;i<128;i++){
        printf("{");
        for (int j=0;j<2;j++){
        printf("0x%02x,",mcu_rgb2[i][j]);}
        printf("0x%02x",mcu_rgb2[i][2]);
        printf("},\n");
        
    }

    return 0;
}*/