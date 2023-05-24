#ifndef _DECODE_IMAGE_CREATOR_H_
#define _DECODE_IMAGE_CREATOR_H_
#include "jpeg_reader.h"
#include <stdint.h>

/*FUNCTIONS*/
    // FOR PGM 
extern void blackandwhite(uint8_t** MCUs_RGB,struct SOF* sof,char* jpeg_name);

    // FOR PPM
extern void rainbow(uint8_t*** MCU_RGB,struct SOF* sof,char* jpeg_name);

#endif /*_DECODE_IMAGE_CREATOR_H_*/