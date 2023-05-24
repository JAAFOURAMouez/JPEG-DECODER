
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../include/jpeg_reader.h"
#include "../include/bitstream.h"
#include "../include/bitstream_to_mcu.h"
#include "../include/extraction_mcu.h"
#include "../include/izz.h"
#include "../include/idct.h"
#include "../include/treatment.h"
#include "../include/toRGB.h"
#include "../include/decoded_image_creator.h"
#include "../include/upsample.h"


int main(int argc,char** argv){

    /* If there is no image.jpeg input : USAGE ERROR*/
    if (argc!=2){
        fprintf(stderr,"Usage: %s fichier.jpeg\n",argv[0]);
        return EXIT_FAILURE;
    }
    /* Check if the image is in jpeg or jpg format */
    size_t len = strlen(argv[1]);
    if (len < 5) {
        /* the string is too short to end with ".jpeg" or ".jpg" */
        return EXIT_FAILURE;
    }
    /* Return Error in this case */
    const char* suffix1 = argv[1] + len - 4;
    const char* suffix2 = argv[1] + len - 5;
    if (!(strcmp(suffix2, ".jpeg") == 0 || strcmp(suffix1, ".jpg") == 0)){
        fprintf(stderr,"Wrong Format: PLEASE INSERT JPEG IMAGE\n ");
        return EXIT_FAILURE;
    }
    FILE *jpeg_image= fopen(argv[1],"r");

    struct HEADER* header=calloc(1,sizeof(struct HEADER));

    printf("\n");

    printf("[HEADER] ON IT !\n");
    extract_header(header,jpeg_image);
    printf("HEADER EXTRACTION ... DONE !\n");
    printf("\n");


    printf("[BITSTREAM EXTRACTION] ON IT!\n");
    create_stream(jpeg_image);
    char* bitstream=bitstream_extraction();
    printf("BITSTREAM EXTRACTED .. NEXT STEP : BLOCKS EXTRACTION\n");
    printf("\n");
    

    uint8_t* pre_order_list=components_order(header->sof,header->sos);
    uint8_t* pre_occurrence_list=block_number_list(header->sof);
    
    huffnode** hufftrees=malloc(2*header->sof->components_number*sizeof(huffnode*));
    if (header->sof->components_number==1){
        hufftrees[0]=header->dhts->dht_table[0]->tree;
        hufftrees[1]=header->dhts->dht_table[1]->tree;
    } else {
        for (int i=0;i<2*header->sof->components_number-2;i++){
            hufftrees[i]=header->dhts->dht_table[i]->tree;
        }
        hufftrees[2*header->sof->components_number-2]=header->dhts->dht_table[2*header->sof->components_number-4]->tree;
        hufftrees[2*header->sof->components_number-1]=header->dhts->dht_table[2*header->sof->components_number-3]->tree;
    }
    
    uint8_t components_number=header->sof->components_number;

    // test lmcu
    
    printf("[EXTRACTING BLOCKS]  TOO FAST ? !\n");
    LMCU* lmcu=bit_stream_to_LMCU(bitstream, pre_order_list, pre_occurrence_list, hufftrees,components_number,header->sof->height, header->sof->width,header->sof); 
    printf("BLOCKS EXTRACTION DONE !\n");
    printf("\n");
    free(bitstream);
    // free hufftrees 
    if(header->sof->components_number==1){
    free_hufftree(hufftrees[0]);
    free_hufftree(hufftrees[1]);
    }else{
    for(uint8_t i=0;i<2*header->sof->components_number-2;i++){
    free_hufftree(hufftrees[i]);
    }
    }
    free(hufftrees);

    //QUANTIFICATION AND ZIZGAG 
    
    printf("[IQZZ] ON IT !");
    M_LMCU* m_lmcu= create_M_LMCU( lmcu, header->dqts , header->sof->components_number,header->sof);
    printf("IQZZ DONE !\n");
    printf("\n");

    // INVERSE DISCRETE COSINUS TRANSFORMATION 
    
    printf("[IDCT] ON IT !!\n");
    iM_LMCU* im_lmcu=create_iM_LMCU(m_lmcu,header->sof);
    printf("IDCT DONE !\n");
    printf("\n");

    
    
    // UPSAMPLING
    iM_LMCU* result;
    if (components_number==3){
        result= up_sample(im_lmcu,header->sof);
    }

    
    printf("UPSAMPING SUCCESSFUL  !\n");
    printf("\n");

    

    // RGB TRANSFORMATION

    printf("[RGB CONVERSION] ON IT !\n");
    uint32_t horizontal_blocks_in_mcu= header->sof->sampling_horizontal[0];
    uint32_t vertical_blocks_in_mcu=header->sof->sampling_vertical[0];
            
            // LIST OF RGB_MCUs
    if (components_number==1){

        uint8_t** MCUs_RGB=malloc(im_lmcu->MCU_counter*sizeof(uint8_t*));
        for (uint32_t i=0;i<im_lmcu->MCU_counter;i++){
            uint8_t* MCU_RGB=malloc(64*sizeof(uint8_t));
            one_Y_mcu_to_rgb(im_lmcu->iM_MCUs[i],MCU_RGB,horizontal_blocks_in_mcu,vertical_blocks_in_mcu);
            MCUs_RGB[i]=MCU_RGB;
        }
        printf("RGB CONVERSION DONE !\n");
        printf("\n");
        //PGM CREATION (for now) 
        printf("LAST STEP ! CREATING MAGICAL IMAGE\n");
        blackandwhite(MCUs_RGB,header->sof,argv[1]);

    }

    else {

        uint8_t*** MCUs_RGB=malloc(result->MCU_counter*sizeof(uint8_t**));
        for (uint32_t i=0;i<result->MCU_counter;i++){
            uint8_t** MCU_RGB=malloc(horizontal_blocks_in_mcu*vertical_blocks_in_mcu*64*sizeof(uint8_t*));
            for (uint32_t j=0;j<horizontal_blocks_in_mcu*vertical_blocks_in_mcu*64;j++){
                    MCU_RGB[j]=malloc(3*sizeof(uint8_t));
            }
            one_YCbCr_mcu_to_rgb(result->iM_MCUs[i],MCU_RGB,horizontal_blocks_in_mcu,vertical_blocks_in_mcu);
            MCUs_RGB[i]=MCU_RGB;
            
        }
        printf("\n");
        printf("RGB CONVERSION DONE !\n");
        printf("\n");
        printf("LAST STEP ! CREATING MAGICAL IMAGE\n");
        rainbow(MCUs_RGB,header->sof,argv[1]);


        for (uint32_t i=0;i<result->MCU_counter;i++){
            for (uint32_t j=0;j<horizontal_blocks_in_mcu*vertical_blocks_in_mcu*64;j++){
                    free(MCUs_RGB[i][j]);
            }
            free(MCUs_RGB[i]);
            
        }
        free(MCUs_RGB);
    }
        

   

    
    printf("JPEG DECOMPRESSION SUCCESSFUL ! THANKS FOR TRUSTING OUR TOOL\n");
    printf("\n");
/*if (header->sof->components_number==3){
    for(uint32_t i=0; i<im_lmcu->MCU_counter ;i++){
        for(uint8_t j=0;j<header->sof->sampling_horizontal[0]*header->sof->sampling_vertical[0];j++){
            for (uint8_t k=0;k<8;k++){
                for(uint8_t c=0;c<8;c++){
                    free(result->iM_MCUs[i]->LY[j]->content[k][c]);
                    free(result->iM_MCUs[i]->LCb[j]->content[k][c]);
                    free(result->iM_MCUs[i]->LCr[j]->content[k][c]);
                }      
                free(result->iM_MCUs[i]->LY[j]->content [k]);
                free(result->iM_MCUs[i]->LCb[j]->content[k]);
                free(result->iM_MCUs[i]->LCr[j]->content[k]);
            }
            free(result->iM_MCUs[i]->LY[j]->content );
            free(result->iM_MCUs[i]->LCb[j]->content);
            free(result->iM_MCUs[i]->LCr[j]->content);
            free(result->iM_MCUs[i]-> LY[j]);
            free(result->iM_MCUs[i]->LCb[j]);
            free(result->iM_MCUs[i]->LCr[j]);
        }
        free(result->iM_MCUs[i]-> LY);
        free(result->iM_MCUs[i]->LCb);
        free(result->iM_MCUs[i]->LCr);

        free(result->iM_MCUs[i]);
    }
}
else{
        for(uint32_t i=0; i<im_lmcu->MCU_counter ;i++){
        for(uint8_t j=0;j<header->sof->sampling_horizontal[0]*header->sof->sampling_vertical[0];j++){
            for (uint8_t k=0;k<8;k++){
                for(uint8_t c=0;c<8;c++){
                    free(im_lmcu->iM_MCUs[i]->LY[j]->content[k][c]);

                }      
                free(im_lmcu->iM_MCUs[i]->LY[j]->content [k]);

            }
            free(im_lmcu->iM_MCUs[i]->LY[j]->content );
            free(im_lmcu->iM_MCUs[i]-> LY[j]);

        }
        free(im_lmcu->iM_MCUs[i]-> LY);

        free(im_lmcu->iM_MCUs[i]);
    }

}
    free(im_lmcu->iM_MCUs);
    free(im_lmcu);
*/
    //free(result);
    free_header(header);
    return EXIT_SUCCESS;
    
}
