#ifndef _TO_RGB_H_
#define _TO_RGB_H_
#include <stdint.h>
#include "idct.h"


/*FONCTIONS*/

extern void one_YCbCr_mcu_to_rgb( iM_MCU* iM_MCU_YCbCr,uint8_t** mcu_rgb,uint32_t nb_horz_blocks_in_mcu,uint32_t nb_vertical_blocks_in_mcu);
extern void one_Y_mcu_to_rgb(iM_MCU* IMCU_Y,uint8_t* mcu_rgb,uint32_t nb_horz_blocks_in_mcu,uint32_t nb_vertical_blocks_in_mcu);
        
#endif /*_TO_RGB_H_*/