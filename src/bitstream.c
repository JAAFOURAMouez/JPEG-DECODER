#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>



/* EXTRACT BITSTREAM INTO A STRING IN ORDER TO READ BIT BY BIT */

void create_stream(FILE* file){

    
    /*CREATES BITSTREAM TEXT AND ELIMINATE BYTESTUFFING*/
    
    FILE* bitstream=fopen("bitstream.txt","wb");
    uint8_t byte, next_byte;
    while (fread(&byte, sizeof(uint8_t), 1, file) == 1){
        if (byte == 0xff){
            if (fread(&next_byte, sizeof(uint8_t), 1, file) == 1){
                if (next_byte == 0x00){
                    
                    /* Byte stuffing found, skip 0x00 and write only 0xff */
                    /*last_byte_ff = true;*/
                    /*Print 0xff */
                   
                    for (int i = 0; i < 8; i++) {
                            int bit = (0xff >> (7 - i)) & 1;
                            fprintf(bitstream, "%d", bit);
                    }
                   
                    continue;
                } else if (next_byte == 0xd9){
                    /* End of image marker found, stop processing*/
                    break;
                } else {
                    /* No byte stuffing, write both bytes to output*/
                   
                        
                        /*Write the bits in the .txt file*/
                        
                        for (int i = 0; i < 8; i++) {
                            int bit = (0xff >> (7 - i)) & 1;
                            fprintf(bitstream, "%d", bit);
                        }
                       
                        for (int i = 0; i < 8; i++) {
                        int bit = (next_byte >> (7 - i)) & 1;
                        fprintf(bitstream, "%d", bit);
                        }
                       

                       
                        
                    }
                   
                    /*Write the bits in the .txt file*/
                 
                    
                     
                   
                   
                }
            }

            
         else {
            /* No byte stuffing, write byte to output*/
               
                /*Write the bits in the .txt file*/
               
                for (int i = 0; i < 8; i++) {
                    int bit = (byte >> (7 - i)) & 1;
                    fprintf(bitstream, "%d", bit);
                }
               
               
               
                
            }
            
            /*Write the bits in the .txt file*/
            
            
           
        }
    fclose(bitstream);
    }
    
char* bitstream_extraction(void){
    FILE* bitstream=fopen("bitstream.txt","rb");
    fseek(bitstream, 0, SEEK_END);
    long file_size = ftell(bitstream);
    fseek(bitstream, 0, SEEK_SET);
    //printf("length: %li\n",file_size);
    char* bitstream_char=(char*)malloc((file_size+1)*sizeof(char));
    fread(bitstream_char, sizeof(char), file_size, bitstream);
    //printf("%s\n",bitstream_char);
    bitstream_char[file_size]='\0';
    fclose(bitstream);

    return bitstream_char;



}
/*void create_stream(FILE* file){
    FILE* bitstream=fopen("bitstream.txt","wb");
    uint8_t byte,next_byte;
    while (fread(&byte,sizeof(uint8_t),1,file)==1){
        if (fread(&next_byte,sizeof(uint8_t),1,file)==1 && next_byte==0xd9 && byte==0xff){
            break;
        }
        else{
        for (int i = 0; i < 8; i++) {
                            int bit = (byte >> (7 - i)) & 1;
                            fprintf(bitstream, "%d", bit);
                        }
        for (int i = 0; i < 8; i++) {
                            int next_bit = (next_byte >> (7 - i)) & 1;
                            fprintf(bitstream, "%d", next_bit);
                        }
        }
    }
    fclose(bitstream);
}*/