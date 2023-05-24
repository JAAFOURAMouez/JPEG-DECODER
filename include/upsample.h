#ifndef _UPSAMPLE_H_
#define _UPSAMPLE_H_
#include <stdint.h>
#include "idct.h"
#include "jpeg_reader.h"



extern void modi_mcu(iM_MCU* old_im_mcu ,iM_MCU* up_samp_im_mcu,uint8_t* S_fact );
extern iM_LMCU* up_sample(iM_LMCU* im_lmcu,struct SOF* sof);

#endif /*_UPSAMPLE_H_*/