#ifndef _EXTRACTION_MCU_H_
#define _EXTRACTION_MCU_H_

#include <stdint.h>
#include "jpeg_reader.h"

//FUNCTIONS

extern uint8_t* components_order(struct SOF* sof,struct SOS* sos);
extern uint8_t* block_number_list(struct SOF* sof);

#endif /* _EXTRACTION_MCU_H_ */