#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../include/huffman.h"
// Function that creates a huffnode: either the root or a child, the char_value is used to create the paths
huffnode* create_huffnode(huffnode* parent, char* char_value) {
    huffnode* new_node = (huffnode*)malloc(sizeof(huffnode));
    if (parent == NULL) {
        new_node->D = 0;
        strcpy(new_node->c, "");
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->P = NULL;
        new_node->S = 0x10; /*10  can't be a symbol :
         if it had any it would mean that we would put 00 and then add 00 and that is done by 2x  with x in 0..f*/
    } else {
        new_node->D = parent->D + 1;
        strcpy(new_node->c, parent->c);
        strcat(new_node->c, char_value);
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->P = parent;
        new_node->S = 0x10;
    }
    return new_node;
}
//freez hufftree
void free_hufftree(huffnode* root) {
 if (root == NULL) {
 return;
 }

 free_hufftree(root->left);
 free_hufftree(root->right);
 free(root);
}

// Fucntion that creates the left child// the path of the child is obtained by concatenating a char to the parent's path 0 in this case
huffnode* goleft(huffnode* parent, char* char_value) {
    huffnode* left_child = create_huffnode(parent, char_value);
    parent->left = left_child;
    return left_child;
}

// Fucntion that creates the left child// the path of the child is obtained by concatenating a char to the parent's path 1 in this case
huffnode* goright(huffnode* parent, char* char_value) {
    huffnode* right_child = create_huffnode(parent, char_value);
    parent->right = right_child;
    return right_child;
}
// returns the first parent having a free right child
huffnode* findfreeparent(huffnode* node) {
    if (node->right == NULL && node->left != NULL) {
        return node;
    }
    if (node->P != NULL) {
        node = node->P;
        while (node->right != NULL) {
            node = node->P;
        }
    }
    return node;
}
// Function that creates the the hufftrees (path,symbol)
struct huffnode* huffmancodes(uint8_t* list_length, uint8_t* symbols) {
    struct huffnode* root = create_huffnode(NULL, "");
    int j = 0;
    for (int i = 0; i < 16; i++) {
        while (list_length[i] > 0) {
            root = findfreeparent(root);
            while (root->D < i + 1) {
                if (root->left == NULL) {
                    root = goleft(root, "0");
                }
                else {
                    root = goright(root, "1");
                }
            }
            root->S = symbols[j];
            j++;
            list_length[i]--;
            root = root->P;
            if (list_length[i] > 0) {
                root = goright(root, "1");
                root->S = symbols[j];
                j++;
                list_length[i]--;
            }
        }
    }
    while (root->P != NULL) {
        root = root->P;
    }
    return root;
}
//this a test
/*int main(void){
    uint8_t frequencies[16] = {0,1,5,1,1,1,1,1,1,0,0,0,0,0,0,0};
    uint8_t symbols[12]={0,1,2,3,4,5,6,7,8,9,0xa,0xb};
    struct huffnode* root = huffmancodes(frequencies,symbols);

    // Print the Huffman codes
    printf("path should be 111111110 and is %s \n",root->right->right->right->right->right->right->right->right->left->c);
    printf("symbol should be b and is %x \n",root->right->right->right->right->right->right->right->right->left->S);

    return 0;
}*/
