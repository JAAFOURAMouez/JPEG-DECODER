#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_
#include <stdint.h>
#include <stdio.h>

typedef struct huffnode {
    int D;
    char c[17];
    struct huffnode* left;
    struct huffnode* right;
    struct huffnode* P;
    uint8_t S;
} 
huffnode;

/*FONCTIONS*/
extern huffnode* huffmancodes(uint8_t* list_length,uint8_t* symbols);
extern void free_hufftree(huffnode* root) ;


#endif /*_HUFFMAN_H_*/