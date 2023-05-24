#ifndef _IZZ_H_
#define _IZZ_H_
#include <stdint.h>
#include "jpeg_reader.h"
#include "bitstream_to_mcu.h"

//MATRIX
typedef struct M_block{
    uint8_t block_type; // 0 for Y // 1 for Cb // 2 for Cr
    int16_t*** content; //8*8 matrix
} M_block;

typedef struct M_MCU{
    M_block** LY;
    M_block** LCb;
    M_block** LCr;
    //uint8_t S
} M_MCU;

typedef struct M_LMCU{
    M_MCU** M_MCUs;
    uint8_t* order_list;
    uint8_t* occurrence_list;
    uint32_t MCU_counter;
} M_LMCU;

/*FONCTIONS*/
extern void zz_inverse(int16_t*** mat_zz_inverse,int16_t** tab_freq,struct DQT* dqt_table);
extern void create_M_block(M_block* new_M_Block,block*  block,struct DQT* dqt_table);


#endif /*_IZZ_H_*/