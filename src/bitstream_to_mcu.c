#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/bitstream_to_mcu.h"
#include "../include/jpeg_reader.h"
#include <string.h>
#include <math.h>

// Function to create a block
block* createBlock(uint8_t T, int16_t** block_content) {
    block* newBlock = malloc(sizeof(block));
    newBlock->block_type = T;
    newBlock->content = calloc(64,sizeof(int16_t*));
    for (int i = 0; i < 64; i++) {
        newBlock->content[i]=block_content[i];
    }
    return newBlock;
}

// Function to create an MCU
MCU* createMCU(block** LY, block** LCb, block** LCr/*, int S*/) {
    MCU* newMCU = malloc(sizeof(MCU));
    newMCU->LY = LY;
    newMCU->LCb = LCb;
    newMCU->LCr = LCr;
    /*newMCU->S = S;*/
    return newMCU;
}

// Function to create an LMCU
LMCU* createLMCU(MCU** MCUs, uint8_t* order_list, uint8_t* occurrence_list, uint32_t MCU_counter) {
    LMCU* new_LMCU = malloc(sizeof(LMCU));
    new_LMCU->MCUs = MCUs;
    new_LMCU->order_list = order_list;
    new_LMCU->occurrence_list = occurrence_list;
    new_LMCU->MCU_counter=MCU_counter;
    return new_LMCU;
}


// Function used to find the corresponding code to(index,magnitude)
int16_t find_code(char* index, uint8_t magnitude) {
    
    int16_t decimal = 0;
    uint8_t power = magnitude - 1;

    if (index[0] == '0') {
        decimal = -((int16_t)(1<<magnitude) - 1);

    }

    for (int i = 0; i < magnitude; i++) {
        char digit = index[i];
        if (digit == '1') {
            decimal += (int16_t)(1<<power);
        }
        power -= 1;
    }

    return decimal;
}
// Function gives a list of the orderd types 
uint8_t* typess(uint8_t *occurence_list, uint8_t *order_list) {
    /*example: occurence_list=4,2,1
               order_list=1,0,3
    -->types:  1,1,0,0,0,0,3
    */
    uint8_t i0= order_list[0];
    uint8_t i1= order_list[1];
    uint8_t i2= order_list[2];
    uint8_t j0= occurence_list[i0];
    uint8_t j1= occurence_list[i1];
    uint8_t j2= occurence_list[i2];
    uint8_t* types = calloc((j0 + j1 + j2) , sizeof(uint8_t));
    for (uint8_t i = 0; i < j0; i++){
        types[i]=i0;
    }
    for (uint8_t i =j0; i < j0+j1; i++){
        types[i]=i1;
    }
    for (uint8_t i = j0+j1; i < j0+j1+j2; i++){
        types[i]=i2;
    }
    return types;
    
}

// Function that returns the firs occurence of i in lst 
uint8_t first_occurrence(uint8_t i, uint8_t *lst, uint8_t lst_length) {
    /*precondition: i is in lst*/
    for (uint8_t index = 0; index < lst_length; index++) {
        if (lst[index] == i) {
            return index;
        }
    }
    return -1; // Element not found, returning a special value (-1)
}



// Function that returns a pointer to an LMCU which represents the list of MCUS given a certain bitstream and other parameters including the correct huffman trees
LMCU* bit_stream_to_LMCU(char* BS, uint8_t* pre_order_list, uint8_t* pre_occurrence_list, huffnode** hufftrees,uint8_t components_number,uint16_t height, uint16_t width,struct SOF* sof) {
    uint32_t i = 0;
    uint8_t* order_list =calloc(3,sizeof(uint8_t));
    uint8_t* occurrence_list =calloc(3,sizeof(uint8_t));
    if (components_number==1){
        order_list[0] = 1;
        order_list[1] = 0;
        order_list[2] = 0;
        occurrence_list[0] = pre_occurrence_list[0];
        occurrence_list[1] = 0;
        occurrence_list[2] = 0;
    }
    else{
        order_list[0] = pre_order_list[0];
        order_list[1] = pre_order_list[1];
        order_list[2] = pre_order_list[2];
        occurrence_list[0] = pre_occurrence_list[0];
        occurrence_list[1] = pre_occurrence_list[1];
        occurrence_list[2] = pre_occurrence_list[2];
    }

    uint8_t* types = typess(occurrence_list, order_list);
    uint32_t MCU_counter = 0;
    uint8_t blocks_in_MCU = 0;
    for (int k = 0; k < 3; k++) {
        blocks_in_MCU += occurrence_list[k];
    }    
    uint32_t total_nbr_MCU = ((uint32_t)(ceil((double)width/(double)(8*sof->sampling_horizontal[0]))) *(uint32_t)(ceil((double)height/(double)(8*sof->sampling_vertical[0]))));
    MCU** MCU_list = calloc(total_nbr_MCU,sizeof(MCU*));
    for (uint32_t k = 0; k < total_nbr_MCU; k++) {
    MCU_list[k] = malloc(sizeof(MCU));
    }
    uint8_t MCU_detector;
    block** list_of_blocks ;
    int16_t ** block_list;
    

    uint8_t type_;    
    uint8_t block_detector;
    huffnode* root;
    uint8_t null ;
    uint8_t mag ;
    int16_t DCy, DCb, DCr;
    int16_t NotDC;
    int16_t Ac ;
    block** Y_list ;
    block** Cb_list;
    block** Cr_list;
    
    MCU* new_MCU;
    int Y_counter ,Cb_counter,Cr_counter;
    char* index;

    while (i < strlen(BS) - 7)/*note that the length of BS is multiple of 8 
                                this is done to treat the case where the length effective bitstream isn't a multiple of 8 */ {
        MCU_detector = 0;   
        list_of_blocks = malloc(blocks_in_MCU * sizeof(block*));
        //one mcu
        while (MCU_detector < blocks_in_MCU) {
            type_ = types[MCU_detector];
            block_detector = 0;
            block_list = malloc(64 * sizeof(int16_t*));
            for (int i = 0; i < 64; i++) {
                block_list[i] = malloc(sizeof(int16_t));
            }    
            //one block
            while (block_detector < 64) {
                if (block_detector == 0) {
                    // DC
                    root = hufftrees[2 * type_];
                    while (!(root->left == NULL && root->right == NULL)) {
                        char bit = BS[i++];
                        if (bit == '0') {
                            root = root->left;
                        } else {
                            root = root->right;
                        }
                    }
                    null=0;
                    mag=0;
                    if (root->S != 0) {
                        null = (root->S )>>4;
                        mag = (root->S) &(0x0f);
                    }
                    index = malloc((mag + 1) * sizeof(char));
                    for (int j = 0; j < mag; j++) {
                        index[j] = BS[i++];
                    }
                    index[mag] = '\0';

                    
                    if (MCU_counter == 0 && (MCU_detector == first_occurrence(0, types, blocks_in_MCU) ||
                        MCU_detector == first_occurrence(1, types, blocks_in_MCU) ||
                        MCU_detector == first_occurrence(2, types, blocks_in_MCU))) {
                        //DC0
                        if (type_ == 0) {
                            DCy = find_code(index, mag);
                            *(block_list[block_detector]) = DCy;
                        } else if (type_ == 1) {
                            DCb = find_code(index, mag);
                            *(block_list[block_detector]) = DCb;
                        } else {
                            DCr = find_code(index, mag);
                            *(block_list[block_detector]) = DCr;
                        }
                        
                    } else {
                        //DCi
                        NotDC = find_code(index, mag);
                        if (type_ == 0) {
                            DCy += NotDC;
                            *(block_list[block_detector]) = DCy;
                        } else if (type_ == 1) {
                            DCb += NotDC;
                            *(block_list[block_detector]) = DCb;
                        } else {
                            DCr += NotDC;
                            *(block_list[block_detector]) = DCr;
                        }
                    }
                    free(index);
                    block_detector++;
                } 
                else {
                    // AC
                    root = hufftrees[2 * type_ + 1];
                    while (!(root->left == NULL && root->right == NULL)) {
                        char bit = BS[i++];
                        if (bit == '0') {
                            root = root->left;
                        } else {
                            root = root->right;
                        }
                    }
                    null = 0;
                    mag = 0;
                    if (root->S != 0) {
                        null = (root->S )>>4;
                        mag = (root->S) &(0x0f);
                    }
                    if (null == 0 && mag == 0){
                        //EOB
                        for (int j = block_detector; j < 64; j++) {
                            *(block_list[block_detector++]) = 0;
                        }
                    }
                    else{
                        for (int j = 0; j < null; j++) {
                            *(block_list[block_detector++]) = 0;
                        }
                        if (mag != 0) {
                            index = malloc((mag + 1) * sizeof(char));
                            for (int j = 0; j < mag; j++) {
                                index[j] = BS[i++];
                            }
                            index[mag] = '\0';
                            Ac = find_code(index, mag);
                            free(index);
                            *(block_list[block_detector++]) = Ac;
                        } else {
                            *(block_list[block_detector++]) = 0;
                        }
                    }
                }
        }
        
        list_of_blocks[MCU_detector] =createBlock(type_,block_list);
        MCU_detector++;

        free(block_list);
    }

    
        Y_counter = 0;
        Cb_counter = 0;
        Cr_counter = 0;
        Y_list = malloc(occurrence_list[0] * sizeof(block*));
        if (components_number==3){
        Cb_list = malloc(occurrence_list[1] * sizeof(block*));
        Cr_list = malloc(occurrence_list[2] * sizeof(block*));
        }
        for (uint32_t counter = 0; counter < blocks_in_MCU; counter++) {
            if (types[counter] == 0) {
                Y_list[Y_counter++] = list_of_blocks[counter];
            }
            else if (types[counter] == 1) {
                Cb_list[Cb_counter++] = list_of_blocks[counter];
            } 
            else {
                Cr_list[Cr_counter++] = list_of_blocks[counter];
            }

        }
        new_MCU=createMCU(Y_list,  Cb_list, Cr_list  );

        MCU_list[MCU_counter] = new_MCU;

        MCU_counter++;
        free(list_of_blocks);
}

LMCU* MCU_lis = createLMCU(MCU_list,  order_list,  occurrence_list,MCU_counter);

//freez
free(types);
//hufftrees freed in main
return MCU_lis;
}


/*int main(void){
    //test block type
    /*uint8_t block_content[64];
    for (int i = 0; i < 64; i++) {
        block_content[i]=i;
    }
    block* newBlock=createBlock(0,block_content);*/
    /*
    //test MCU_type
    block* LY[2];
    
    
    block** LCb =calloc(1,occurence_list[1],sizeof(block));
    block* LCr[1];

    uint8_t block_content[64];
    //block* newBlock;
    for (int i = 0; i < 64; i++) {
        block_content[i]=i; 
    }
    LY[0]=createBlock(0,block_content);

    for (int i = 0; i < 64; i++) {
        block_content[i]=(i+64); 
    }

    LCb[0]=createBlock(0,block_content);
    
    for (int i = 0; i < 64; i++) {
        printf("voici LCb[%i] expected %i but is %u \n",i,i+64,LCb[0]->content[i]);
    }*/

    //MCU*  new_mcu = createMCU( LY,  LCb,  LCr)
    /*//test of find code
    char* index ="11111";
    int8_t code=find_code(index,5);
    printf("code is %d \n",code);*/
    
    //test of typess
    /*uint8_t occurence_list[]={4,2,2};
    uint8_t order_list[]={2,1,0};
    uint8_t* types=typess(occurence_list, order_list);
    for (uint8_t i = 0; i < 8; i++){
        printf("%u,; ",types[i]);
    }
    printf("\n");*/
    //test of find_position
    /*uint8_t order_list[]={2,1,0}; 
    printf("find_position(order_list,0): %u \n",find_position(order_list,5));*/
    
    //invader works just fine
    /*char* BS="01111100110100011100101011001010110111000111011011011010010011010110101000000000000101011110110101000001111100010010110100111010110111000111000010001011000101101011111001001100110010011011101110110011010011111111101100110101101110000111110100010011101010110001001010011101000011110000111000011111010011100001110111100011";
    uint8_t pre_order_list[1]={1};
    uint8_t pre_occurrence_list[1]={1};
    uint8_t components_number=1;
    uint8_t height=8;
    uint8_t width=8;
    uint8_t shapee[2]={8,8};
    
    uint8_t l1[16]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    uint8_t l2[16]={0,2,1,2,7,0,0,0,0,0,0,0,0,0,0,0};
    uint8_t l11[1]={7};
    uint8_t l22[12]={0x17,0x18,0x15,0x08,0x19,0,0x9,0x13,0x23,0x28,0x29,0x37};*/
    
    //gris
    /*uint8_t pre_order_list[1]={1};
    uint8_t pre_occurrence_list[1]={1};
    uint8_t components_number=1;
    uint16_t height=320;
    uint16_t width=320;
    uint8_t shapee[2]={8,8};
    uint8_t l1[16]={0,2,2,2,3,1,0,0,0,0,0,0,0,0,0,0};
    uint8_t l2[16]={0,2,2,0,4,3,6,4,3,8,1,4,0,5,5,0};
    uint8_t l11[10] = {3, 4, 1, 2, 0, 5, 6, 7, 8, 9};
    uint8_t l22[49] = {0x01, 0x02, 0x03, 0x11, 0x00, 0x04, 0x12, 0x21, 0x05, 0x31, 0x41, 0x06, 0x07, 0x13, 0x22, 0x51, 0x61, 0x08, 0x71, 0x81, 0x91, 0x14, 0x32, 0xA1, 0x09, 0x23, 0x42, 0xB1, 0xC1, 0xD1, 0xE1, 0xF0, 0xF1, 0x15, 0x33, 0x52, 0x62, 0x24, 0x43, 0x44, 0x72, 0x92, 0x16, 0x17, 0x18, 0x25, 0x82};
    
    */
/*
    //poupidou
    uint8_t pre_order_list[3]={0,1,2};
    uint8_t pre_occurrence_list[3]={2,1,1};
    uint8_t components_number=3;
    uint16_t height=16;
    uint16_t width=16;
    uint8_t shapee[2]={16,8};
    uint8_t l1[16]={0,1,5,1,1,1,1,1,1,0,0,0,0,0,0,0};
    uint8_t l2[16]={0,2,1,3,3,2,4,3,5,5,4,4,0,0,1,125};
    uint8_t l3[16]={0,3,1,1,1,1,1,1,1,1,1,0,0,0,0,0};
    uint8_t l4[16]={0,2,1,2,4,4,3,4,7,5,4,4,0,1,2,119};
    uint8_t l5[16]={0,3,1,1,1,1,1,1,1,1,1,0,0,0,0,0};
    uint8_t l6[16]={0,2,1,2,4,4,3,4,7,5,4,4,0,1,2,119};
    uint8_t l11[12]={0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb};
    uint8_t l22[162]={0x1, 0x2, 0x3, 0x0, 0x4, 0x11, 0x5, 0x12, 0x21, 0x31, 0x41, 0x6, 0x13, 0x51, 0x61, 0x7, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x8, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x9, 0xa, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa};
    uint8_t l33[12]= {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb};
    uint8_t l44[162]={0x0, 0x1, 0x2, 0x3, 0x11, 0x4, 0x5, 0x21, 0x31, 0x6, 0x12, 0x41, 0x51, 0x7, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x8, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x9, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0xa, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa};
    uint8_t l55[12]= {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb};
    uint8_t l66[162]={0x0, 0x1, 0x2, 0x3, 0x11, 0x4, 0x5, 0x21, 0x31, 0x6, 0x12, 0x41, 0x51, 0x7, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x8, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x9, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0xa, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa};
    char* BS="111011000011111000011110000110000010010110001111011010101100110011110110101011001001100101011101010111100001010011000000011010111000100111011111011010010101011011110011101100100111111111010101110000111000111111101110111101011110011010011101111100010000110001101000110011101010110001010110000101001001001011101111111001001000100100100111101101110110000111111111001011011010010011100011110011101000111100111111110000101011111111111101011010101111100101011111011000010011010100101110011110000101011111101011110010110011111100111000101101111111111001010110001111101101111000111000100001111100110101111110011111101011110010101111110100001110010001111100001010111110001000000100101101011101110000100011110100100011010010000110100101110010000011001101011101010011101100101010111110011101101001111011100000111011101111001100100110011111111111100111100110011100010101110001101010111010110111001001011101110111100100101111111101100101010011110110101100110110100110101010110010000010110101101111000101010001010001111101101111011010110111100001001000001001000010110001101010101000110011001011001010011100111001000110000001110001111100101111110010110101111101000101111011001111111110011001101111101110000100111010001011111001111011001011101111110011111111001000";
    huffnode** hufftrees=calloc(6,sizeof(huffnode*));
    hufftrees[0]=huffmancodes(l1,l11);
    hufftrees[1]=huffmancodes(l2,l22);
    hufftrees[2]=huffmancodes(l3,l33);
    hufftrees[3]=huffmancodes(l4,l44);
    hufftrees[4]=huffmancodes(l5,l55);
    hufftrees[5]=huffmancodes(l6,l66);
    //printf("path should be 100 and is %s \n",root->left->left->right->c);
    //printf("symbol should be b and is %x \n",root->right->right->right->right->right->right->right->right->left->S);

    LMCU* lmcu= bit_stream_to_LMCU(BS,pre_order_list,pre_occurrence_list,hufftrees,components_number,height,width,shapee);
    printf("Cb\n");
    for (uint8_t i = 0; i < 64; i++) {
        printf("/%x",*(lmcu->MCUs[0]->LCb[0]->content[i]));

        
    }    
    printf("Cr\n");
    for (uint8_t i = 0; i < 64; i++) {
        printf("/%x",*(lmcu->MCUs[0]->LCr[0]->content[i]));      
    }    
    
}*/