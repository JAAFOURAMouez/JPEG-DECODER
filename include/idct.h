#ifndef _IDCT_H_
#define _IDCT_H_
#include <stdint.h>
#include <stdio.h>
#include "izz.h"

typedef struct iM_block{
    uint8_t block_type; // 0 for Y // 1 for Cb // 2 for Cr
    uint8_t*** content; //8*8 matrix
} iM_block;

typedef struct iM_MCU{
    iM_block** LY;
    iM_block** LCb;
    iM_block** LCr;
    //uint8_t S
} iM_MCU;

typedef struct iM_LMCU{
    iM_MCU** iM_MCUs;
    uint8_t* order_list;
    uint8_t* occurrence_list;
    uint32_t MCU_counter;
} iM_LMCU;

extern void fast_idct(int16_t*** izz_matrix,uint8_t*** out_matrix);
extern void create_iM_block(iM_block* new_iM_Block, M_block*  M_block);

#endif /*_IDCT_H_*/
