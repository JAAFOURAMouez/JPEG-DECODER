#ifndef _BITSTREAM_TO_MCU_H
#define _BITSTREAM_TO_MCU_H
#include <stdint.h>
#include "huffman.h"
#include "jpeg_reader.h"
typedef struct block{
    uint8_t block_type; // 0 for Y // 1 for Cb // 2 for Cr
    int16_t** content;
} block;

typedef struct MCU{
    block** LY;
    block** LCb;
    block** LCr;
    //uint8_t S
} MCU;

typedef struct LMCU{
    MCU** MCUs;
    uint8_t* order_list;
    uint8_t* occurrence_list;
    uint32_t MCU_counter;
} LMCU;


extern LMCU* bit_stream_to_LMCU(char* BS, uint8_t* pre_order_list, uint8_t* pre_occurrence_list, huffnode** hufftrees,uint8_t components_number,uint16_t height, uint16_t width,struct SOF* sof);



#endif /*_BITSTREAM_TO_MCU_H */