#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../include/izz.h"
#include "../include/bitstream_to_mcu.h"
#include "../include/idct.h"


void create_M_MCU(M_MCU* new_M_MCU ,MCU* mcu,uint8_t* occ_list,struct DQTs* DQT_list,uint8_t components_number,struct SOF* sof){



    uint8_t Y_iq_index,Cb_iq_index,Cr_iq_index;


    uint8_t Y_index,Cb_index,Cr_index;

    if (components_number ==3){
        new_M_MCU->LY  =calloc(occ_list[0],sizeof(M_block*));
        new_M_MCU->LCb =calloc(occ_list[1],sizeof(M_block*));
        new_M_MCU->LCr =calloc(occ_list[2],sizeof(M_block*));
        Y_iq_index=sof->quantification_table_i_q[0];
        Cb_iq_index=sof->quantification_table_i_q[1];
        Cr_iq_index=sof->quantification_table_i_q[2];
        for (uint8_t i=0; i<DQT_list->dqt_counter;i++){
            if (DQT_list->dqt_table[i]->i_q==Y_iq_index){
                Y_index=i;
            }
            if (DQT_list->dqt_table[i]->i_q==Cb_iq_index){
                Cb_index=i;
            }
            if (DQT_list->dqt_table[i]->i_q==Cr_iq_index){
                Cr_index=i;
            }
        }

        for (uint8_t i=0; i<occ_list[0];i++){
            new_M_MCU->LY[i]=malloc(sizeof(M_block));
            create_M_block(new_M_MCU->LY[i],mcu->LY[i],DQT_list->dqt_table[Y_index]); 
        }
        for (uint8_t i=0; i<occ_list[1];i++){
            new_M_MCU->LCb[i]=malloc(sizeof(M_block));
            create_M_block(new_M_MCU->LCb[i],mcu->LCb[i],DQT_list->dqt_table[Cb_index]);         
        }
        for (uint8_t i=0; i<occ_list[2];i++){
            new_M_MCU->LCr[i] = malloc(sizeof(M_block));
            create_M_block(new_M_MCU->LCr[i] ,mcu->LCr[i],DQT_list->dqt_table[Cr_index]);
        }
        free(mcu->LY);
        free(mcu->LCb);
        free(mcu->LCr);
        free(mcu);
    }
    else{
        Y_iq_index=sof->quantification_table_i_q[0];
        new_M_MCU->LY =calloc(occ_list[0],sizeof(M_block*));

        for (uint8_t i=0; i<DQT_list->dqt_counter;i++){
            if (DQT_list->dqt_table[i]->i_q==Y_iq_index){
                Y_index=i;
            }       
        }
        for (uint8_t i=0; i<occ_list[0];i++){
                new_M_MCU->LY[i]=malloc(sizeof(M_block));
                create_M_block(new_M_MCU->LY[i],mcu->LY[i],DQT_list->dqt_table[Y_index]); 
        }     
        free(mcu->LY);  
    }
}

M_LMCU* create_M_LMCU(LMCU* lmcu,struct DQTs* DQT_list,uint8_t components_number,struct SOF* sof){

    M_LMCU* new_M_LMCU = malloc(sizeof(M_LMCU));

    new_M_LMCU->occurrence_list=lmcu->occurrence_list;
    new_M_LMCU->order_list=lmcu->order_list;
    new_M_LMCU->MCU_counter=lmcu->MCU_counter;
    new_M_LMCU->M_MCUs= calloc(new_M_LMCU->MCU_counter,sizeof(M_MCU*));

    for(uint32_t i=0; i<lmcu->MCU_counter ;i++){
        new_M_LMCU->M_MCUs[i]=malloc(sizeof(M_MCU));
        create_M_MCU(new_M_LMCU->M_MCUs[i],lmcu->MCUs[i],lmcu->occurrence_list, DQT_list,components_number, sof);
        
    }
    free(lmcu->MCUs);
    free(lmcu);

    /*free(lmcu->MCUs);
    free(lmcu);*/
    return new_M_LMCU;
}

//after inverse_discrete_cosinus_transformation

void create_iM_MCU(iM_MCU* new_iM_MCU ,M_MCU* mcu,uint8_t* occ_list,struct SOF* sof){
        if (sof->components_number ==3){
        new_iM_MCU->LY =calloc(occ_list[0],sizeof(iM_block*));
        new_iM_MCU->LCb =calloc(occ_list[1],sizeof(iM_block*));
        new_iM_MCU->LCr =calloc(occ_list[0],sizeof(iM_block*));
        
        for (uint8_t i=0; i<occ_list[0];i++){
            new_iM_MCU->LY[i]=malloc(sizeof(iM_block));
            create_iM_block(new_iM_MCU->LY[i],mcu->LY[i]);           
        }
        for (uint8_t i=0; i<occ_list[1];i++){
            new_iM_MCU->LCb[i]=malloc(sizeof(iM_block));
            create_iM_block(new_iM_MCU->LCb[i],mcu->LCb[i]);            
        }
        for (uint8_t i=0; i<occ_list[2];i++){
            new_iM_MCU->LCr[i] = malloc(sizeof(iM_block));
            create_iM_block(new_iM_MCU->LCr[i] ,mcu->LCr[i]);
        }
        free(mcu->LY);
        free(mcu->LCb);
        free(mcu->LCr);
        free(mcu);
        }

        else {
            new_iM_MCU->LY =calloc(occ_list[0],sizeof(iM_block*));

            for (uint8_t i=0; i<occ_list[0];i++){
            new_iM_MCU->LY[i]=malloc(sizeof(iM_block));
            create_iM_block(new_iM_MCU->LY[i],mcu->LY[i]);   
        }
        free(mcu->LY); 
        }
}   

iM_LMCU* create_iM_LMCU(M_LMCU* m_lmcu,struct SOF* sof){
    
    iM_LMCU* new_iM_LMCU = malloc(sizeof(iM_LMCU));
    new_iM_LMCU->occurrence_list=m_lmcu->occurrence_list;
    new_iM_LMCU->order_list=m_lmcu->order_list;
    new_iM_LMCU->MCU_counter=m_lmcu->MCU_counter;
    new_iM_LMCU->iM_MCUs= calloc(new_iM_LMCU->MCU_counter,sizeof(iM_MCU*));

    for(uint32_t i=0; i<m_lmcu->MCU_counter ;i++){
        new_iM_LMCU->iM_MCUs[i]=malloc(sizeof(iM_MCU));
        create_iM_MCU(new_iM_LMCU->iM_MCUs[i],m_lmcu->M_MCUs[i],m_lmcu->occurrence_list,sof);
    }

    free(m_lmcu->M_MCUs);
    free(m_lmcu);
    return new_iM_LMCU;
}


