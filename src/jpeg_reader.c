#include "../include/jpeg_reader.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h> /* for the function ntohs() */


struct APP0* EXTRACT_APP0(FILE* file){
    /*INITIALIZE APP0 STRUCTURE*/
    struct APP0* app0 = malloc(sizeof(struct APP0));
    /*INITIALIZE SECTION LENGTH*/
    fread(&(app0->section_length), sizeof(uint16_t), 1, file);
    app0->section_length = ntohs(app0->section_length);
    /*INITIALIZE & VERIFY IMAGE_TYPE*/
    app0->image_type = malloc(sizeof(char)*5);
    fread(app0->image_type, sizeof(char), 5, file);
    if (strncmp(app0->image_type, "JFIF", 4) != 0) {
        printf("Warning: image type is not JFIF\n");
        exit(EXIT_FAILURE);
    }
    return app0;
}

struct DQT* EXTRACT_DQT(FILE* file){
    /*INITIALIZE DQT STRUCTURE*/
    struct DQT* dqt = malloc(sizeof(struct DQT));
    /*INITIALIZE SECTION LENGTH*/
    fread(&(dqt->section_length), sizeof(uint16_t), 1, file);
    dqt->section_length = ntohs(dqt->section_length);
    /*INITIALIZE I_Q*/
    fread(&(dqt->i_q),sizeof(uint8_t),1,file);
    if(dqt->i_q>0x01){
        dqt->i_q=dqt->i_q-16;
    }
    /*INITIALIZE QUANTIFICATION VALUES*/
    dqt->quantification_values=calloc(64,sizeof(uint8_t));
    for (int i=0;i<64;i++){
        uint8_t quant_val;
        fread(&quant_val,sizeof(uint8_t),1,file);
        dqt->quantification_values[i] = quant_val;
    }
    return dqt;
}

struct SOF* EXTRACT_SOF(FILE* file){
    /*INITIALIZE SOF STRUCTURE*/
    struct SOF* sof= malloc(sizeof(struct SOF));
    /*INITIALIZE SECTION LENGTH*/
    fread(&(sof->section_length), sizeof(uint16_t), 1, file);
    sof->section_length = ntohs(sof->section_length);
    /*ADVANCE*/
    int pos=ftell(file);
    fseek(file,pos+1,SEEK_SET);
    /*INITIALIZE HEIGHT*/
    fread(&(sof->height),sizeof(uint16_t),1,file);
    sof->height = ntohs(sof->height);
    /*INITIALIZE WIDTH*/
    fread(&(sof->width),sizeof(uint16_t),1,file);
    sof->width = ntohs(sof->width);
    /*INITIALIZE COMPONENTS NUMBER*/
    fread(&(sof->components_number),sizeof(uint8_t),1,file);
    /*INITIALIZE I_C, SAMPLING_H, SAMPLING_V & Q_TABLE*/
    sof->i_c=malloc(sof->components_number*sizeof(uint8_t));
    sof->sampling_horizontal=malloc(sof->components_number*sizeof(uint8_t));
    sof->sampling_vertical=malloc(sof->components_number*sizeof(uint8_t));
    sof->quantification_table_i_q=malloc(sof->components_number*sizeof(uint8_t));
     for (int i=0;i<sof->components_number;i++){
        fread(&(sof->i_c)[i],sizeof(uint8_t),1,file);
        fread(&(sof->sampling_horizontal)[i],sizeof(uint8_t),1,file);
        fread(&(sof->quantification_table_i_q)[i],sizeof(uint8_t),1,file);
        sof->sampling_vertical[i]=(sof->sampling_horizontal[i])%16;
        sof->sampling_horizontal[i]=(sof->sampling_horizontal[i])/16;
     }
     return sof;
}

struct DHT* EXTRACT_DHT(FILE* file){
    
    /*INITIALIZE DHT STRUCTURE*/
    
    struct DHT* dht= malloc(sizeof(struct DHT));
    
    /*INITIALIZE SECTION LENGTH*/
    
    fread(&(dht->section_length), sizeof(uint16_t), 1, file);
    dht->section_length = ntohs(dht->section_length);
    
    /*INITIALIZE HUFFMAN INFORMATION*/
    
    fread(&(dht->huffman_information)[0],sizeof(uint8_t),1,file);
    dht->huffman_information[1]=dht->huffman_information[0]%16;
    dht->huffman_information[0]=dht->huffman_information[0]/16;
    if(dht->huffman_information[0]>1){
        printf("ERROR HUFFMAN INFORMATION\n");
        exit(EXIT_FAILURE);
    }
    
    /*INITIALIZE SYMBOLS NUMBER*/
    
    dht->symbols_number_total=0;
    for (int i=0;i<16;i+=1){
        fread(&(dht->symbols_number)[i],sizeof(uint8_t),1,file);
        dht->symbols_number_total+=dht->symbols_number[i];
    }

    /*INITIALIZE SYMBOLS*/
    
    uint8_t length_symbols_table=dht->section_length-19;
    dht->symbols=calloc(length_symbols_table,sizeof(uint8_t));
    for (int i=0;i<length_symbols_table ;i++) {
        fread(&(dht->symbols)[i],sizeof(uint8_t),1,file);
        //printf("symb: %02x\n",dht->symbols[i]);
    }
    
    /*INITIALIZE TREE*/
    dht->tree = huffmancodes(dht->symbols_number, dht->symbols); //free

    return dht;
}

struct SOS* EXTRACT_SOS(FILE* file){
    /*INITIALIZE SOS STRUCTURE*/
    struct SOS* sos= malloc(sizeof(struct SOS));
    /*INITIALIZE SECTION LENGTH*/
    fread(&(sos->section_length), sizeof(uint16_t), 1, file);
    sos->section_length = ntohs(sos->section_length);
    /*INITIALIZE N*/
    fread(&(sos->components_number), sizeof(uint8_t), 1, file);
    /*INITIALIZE I_C , I_H_DC & I_H_AC*/
    sos->i_c=malloc(sos->components_number*sizeof(uint8_t));
    sos->i_h_DC=malloc(sos->components_number*sizeof(uint8_t));
    sos->i_h_AC=malloc(sos->components_number*sizeof(uint8_t));
    for (int i=0;i<sos ->components_number;i++){
        fread(&(sos->i_c)[i],sizeof(uint8_t),1,file);
        fread(&(sos->i_h_DC)[i],sizeof(uint8_t),1,file);
        sos->i_h_AC[i]=(sos->i_h_DC[i])%16;
        sos->i_h_DC[i]=(sos->i_h_DC[i])/16;
    }

    /* ADVANCE TO START OF ENCODED IMAGE TO START DECODING*/
    int pos=ftell(file);
    fseek(file,pos+3,SEEK_SET);
    
    return sos;

}



void extract_header(struct HEADER* header,FILE* file){
    uint8_t Bytes;
    header->dqts=calloc(1,sizeof(struct DQTs));
    header->dqts->dqt_table=calloc(4,sizeof(struct DQT* ));
    header->dqts->dqt_counter=0;
    header->dhts=calloc(1,sizeof(struct DHTs));
    header->dhts->dht_table=calloc(4,sizeof(struct DHT* ));
    header->dhts->dht_counter=0;
    while(fread(&Bytes,sizeof(uint8_t),1,file)==1){ /*READ 1 BY 1 BYTE*/
        
        if (Bytes==0xff){ /* IF WE HAVE A BEGINNING OF A MARKER FF*/
            fread(&Bytes,sizeof(uint8_t),1,file); /* ADVANCE BY 1 BYTE*/
           
            switch (Bytes) {
                case 0xd8: /* IF MARKER IS SOI D8, CONTINUE */
                    continue;
                case 0xe0: /* IF MARKER IS E0, EXTRACT APP0 */
                    header->app0=EXTRACT_APP0(file);
                    break;
                case 0xdb:/* IF MARKER IS DB, EXTRACT ALL DQT */
                    header->dqts->dqt_table[header->dqts->dqt_counter++]=EXTRACT_DQT(file);
                    break;
                case 0xc0:/* IF MARKER IS C0, EXTRACT SOF */
                    header->sof=EXTRACT_SOF(file);
                    break;
                case 0xc4:/* IF MARKER IS C4, EXTRACT ALL DHT */
                    header->dhts->dht_table[header->dhts->dht_counter++]=EXTRACT_DHT(file); 
                    
                    break;
                case 0xda:/* IF MARKER IS DA, EXTRACT SOS */
                    header->sos=EXTRACT_SOS(file);
                    break;
            }
            if (Bytes == 0xda) { /* IF MARKER IS DA, EXIT WHILE LOOP */
                break;
            }
        }
    }
}


void free_header(struct HEADER* header){
    
    /* I FEEEL FREEEEEEEEEEEEEEEEEEEE */
 
    free(header->app0->image_type);
    free(header->sof->i_c);
    free(header->sof->quantification_table_i_q);
    free(header->sof->sampling_horizontal);
    free(header->sof->sampling_vertical);
    free(header->sof); 
    for (int i=0;i<header->dqts->dqt_counter;i++){
    free(header->dqts->dqt_table[i]->quantification_values);
    free(header->dqts->dqt_table[i]);
    }
    for (int i=0;i<header->dhts->dht_counter;i++){
    free(header->dhts->dht_table[i]->symbols);

    free(header->dhts->dht_table[i]);
    }
    free(header->dhts->dht_table);
    free(header->dqts->dqt_table);
    free(header->dqts);
    free(header->dhts);
    free(header->app0);
    free(header->sos->i_c);
    free(header->sos->i_h_AC);
    free(header->sos->i_h_DC); 
    free(header->sos);
    free(header);
 
}
