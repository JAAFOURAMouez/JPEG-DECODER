#ifndef _JPEG_READER_H_
#define _JPEG_READER_H_
#include <stdio.h>
#include <stdint.h>
#include "huffman.h"


struct HEADER{
    struct APP0* app0;
    struct SOF* sof;
    struct SOS* sos;
    struct DQTs* dqts;
    struct DHTs* dhts;
};

struct DQTs{
    struct DQT** dqt_table;
    int dqt_counter;
};

struct DHTs{
    struct DHT** dht_table;
    int dht_counter;
};

struct APP0 {
    uint16_t section_length;
    char* image_type;
};

struct DQT {
    uint16_t section_length;
    uint8_t i_q;
    uint8_t* quantification_values;
};

struct SOF {
    uint16_t section_length;
    uint16_t height;
    uint16_t width;
    uint8_t components_number;
    uint8_t* i_c;
    uint8_t* sampling_horizontal;
    uint8_t* sampling_vertical;
    uint8_t* quantification_table_i_q;
};

struct DHT{
    uint16_t section_length;
    unsigned char huffman_information[2];
    uint8_t symbols_number[16];
    uint8_t* symbols;
    huffnode* tree;
    uint8_t symbols_number_total;

};

struct SOS{
    uint16_t section_length;
    uint8_t components_number;
    uint8_t* i_c;
    uint8_t* i_h_DC;
    uint8_t* i_h_AC;
};

/*FONCTIONS
struct APP0* extract_app0(FILE* file);
struct DQT* extract_dqt(FILE* file);
struct SOF* extract_sof(FILE* file);
struct DHT* extract_dht(FILE* file);
struct SOS* extract_sos(FILE* file);*/

extern void free_header(struct HEADER* header);
extern void extract_header(struct HEADER* header,FILE* file);



#endif /*_JPEG_READER_H_*/