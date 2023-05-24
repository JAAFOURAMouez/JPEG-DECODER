#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../include/jpeg_reader.h"
#include <math.h>


char * create_name_file(char * jpeg_name, uint8_t nb_components){
    uint8_t length =strlen(jpeg_name);
    char * new_name =malloc((length+1)*sizeof(char));
    strcpy(new_name,jpeg_name);
    uint8_t pos_change_type=length-4;
    if (new_name[pos_change_type]!='.'){
        pos_change_type-=1;
    }
    new_name[pos_change_type+1]='p';
    new_name[pos_change_type+2]='p';
    if(nb_components==1) {
        new_name[pos_change_type+2]='g';
    }
    new_name[pos_change_type+3]='m';
    new_name[pos_change_type+4]='\0';
    return new_name;
}

void blackandwhite(uint8_t** MCUs_RGB,struct SOF* sof,char* jpeg_name){
    
    // INTITIALIZE PGM IMAGE
    char *new_file = create_name_file(jpeg_name,1);
    FILE *new_image = fopen(new_file,"wb");
    char* format="P5";
    char* rgb="255";
    uint32_t MCU_hori_number =(1+(uint32_t)ceil(sof->width/(sof->sampling_horizontal[0]*8)));
    uint32_t MCU_vertical_number=(1+(uint32_t)ceil(sof->height/(sof->sampling_vertical[0]*8)));
    fprintf(new_image,"%s\n",format);
    fprintf(new_image,"%u %u\n",(sof->width),(sof->height));
    fprintf(new_image,"%s\n",rgb);
    uint8_t jmax=8;
    uint8_t kmax=8;

    // WRITE CONTENT IN PGM IMAGE
    if((ceil(sof->width/(sof->sampling_horizontal[0]*8)))!=((double)(sof->width)/(double)((sof->sampling_horizontal[0]*8)))){
        for (uint32_t l=0;l<MCU_vertical_number;l++){
        if (l==MCU_vertical_number-1){jmax=sof->height%8;}
        else{jmax=8;}
            for (int j = 0; j < jmax; j++)
                for (uint32_t i = 0; i < MCU_hori_number ;i++){
                if (i==MCU_hori_number-1){kmax=sof->width%8;}else{kmax=8;}
                    for (int k = 0; k < kmax;k++)
                        fwrite(&(MCUs_RGB[i+l*(MCU_hori_number)][j*8+k]),sizeof(uint8_t),1,new_image);}
            }
        }
    else{
        for (uint32_t l=0;l<MCU_vertical_number-1;l++)
            for (int j = 0; j < 8; j++)
                for (uint32_t i = 0; i < MCU_hori_number -1;i++)
                    for (int k = 0; k < 8;k++)
                        fwrite(&(MCUs_RGB[i+l*(MCU_hori_number-1)][j*8+k]),sizeof(uint8_t),1,new_image);
    }
    
                        
    fclose(new_image);
    free(new_file);


}


void rainbow(uint8_t*** MCUs_RGB,struct SOF* sof,char* jpeg_name){
    //missing cases h*v>2
    char *new_file = create_name_file(jpeg_name,sof->components_number);
    FILE *new_image = fopen(new_file,"wb");
    char* format="P6";
    char* rgb="255";
    fprintf(new_image,"%s\n",format);
    fprintf(new_image,"%u %u\n",sof->width,sof->height);
    fprintf(new_image,"%s\n",rgb);
    

    uint32_t MCU_hori_number     =(1+(uint32_t)ceil(sof->width/(sof->sampling_horizontal[0]*8)));
    uint32_t MCU_vertical_number =(1+(uint32_t)ceil(sof->height/(sof->sampling_vertical[0]*8)));
    uint32_t real_MCU_hori_number=(uint32_t)ceil((double)sof->width/ ((double)sof->sampling_horizontal[0]*8));
    uint8_t jmax;
    uint8_t kmax;
    uint8_t m;
    uint8_t nbr_blocs_per_mcu=sof->sampling_horizontal[0]*sof->sampling_vertical[0];

    //case of h,v<=2
    if(sof->sampling_horizontal[0]<=2 &&sof->sampling_vertical[0]<=2){

        for (uint32_t l=0;l<MCU_vertical_number;l++){
            if (l==MCU_vertical_number-1){jmax=(sof->height)%(sof->sampling_vertical[0]*8);}
            else{jmax=sof->sampling_vertical[0]*8;}
                for (int j = 0; j < jmax; j++)
                for (uint32_t i=0;i<MCU_hori_number;i++)
                    {if (i==MCU_hori_number-1){kmax=(sof->width)%(8*sof->sampling_horizontal[0]);}
                    else{kmax=sof->sampling_horizontal[0]*8;}
                            for (int k = 0; k <kmax;k++){
                                if (k>7 && j<=7 ){m=(1%nbr_blocs_per_mcu);}
                                else if (k<=7 && j>7 ){m=(2%nbr_blocs_per_mcu);
                                    if (nbr_blocs_per_mcu==2){m=1;}}
                                else if (k>7 && j>7 ){m=(3%nbr_blocs_per_mcu);}
                                else{m=0;}
                                fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][0]),sizeof(uint8_t),1,new_image);
                                fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][1]),sizeof(uint8_t),1,new_image);
                                fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][2]),sizeof(uint8_t),1,new_image);
                                }
                        }
            }  
        }
    //case of h,v in {(4,1) ,(1,4),(3,1) ,(1,3 )}
    else if((sof->sampling_horizontal[0]==4 && sof->sampling_vertical[0]==1) || (sof->sampling_horizontal[0]==1 && sof->sampling_vertical[0]==4)
    ||(sof->sampling_horizontal[0]==3 && sof->sampling_vertical[0]==1) || (sof->sampling_horizontal[0]==1 && sof->sampling_vertical[0]==3)){

        for (uint32_t l=0;l<MCU_vertical_number;l++){
        if (l==MCU_vertical_number-1){jmax=(sof->height)%(sof->sampling_vertical[0]*8);}
        else{jmax=sof->sampling_vertical[0]*8;}
            for (int j = 0; j < jmax; j++)
            for (uint32_t i=0;i<MCU_hori_number;i++)
                {if (i==MCU_hori_number-1){kmax=(sof->width)%(8*sof->sampling_horizontal[0]);}
                else{kmax=sof->sampling_horizontal[0]*8;}
                        for (int k = 0; k <kmax;k++){
                            if      ((k<=15 && k>=8)  || (j<=15 && j>=8)){m=1;}
                            else if ((k<=23 && k>=16 )||(j<=23 && j>=16 )){m=2;}
                            else if(k>=24||j>=24){m=3;}
                            else{m=0;}
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][0]),sizeof(uint8_t),1,new_image);
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][1]),sizeof(uint8_t),1,new_image);
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][2]),sizeof(uint8_t),1,new_image);
                            }
                    }
            }    
        }
    //case of h,v == 4,2
    else if(sof->sampling_horizontal[0]==4 && sof->sampling_vertical[0]==2){
        for (uint32_t l=0;l<MCU_vertical_number;l++){
        if (l==MCU_vertical_number-1){jmax=(sof->height)%(sof->sampling_vertical[0]*8);}
        else{jmax=sof->sampling_vertical[0]*8;}
            for (int j = 0; j < jmax; j++)
            for (uint32_t i=0;i<MCU_hori_number;i++)
                {if (i==MCU_hori_number-1){kmax=(sof->width)%(8*sof->sampling_horizontal[0]);}
                else{kmax=sof->sampling_horizontal[0]*8;}
                        for (int k = 0; k <kmax;k++){
                            /* m indicates the bloc
                            0 1 2 3
                            4 5 6 7
                            */
                            if      ((k<=15 && k>=8 ) &&j<=7 ){m=1;}
                            else if ((k<=23 && k>=16) &&j<=7 ){m=2;}
                            else if ((k>=24         ) &&j<=7 ){m=3;}
                            else if ((k<=7          ) &&j<=7 ){m=0;}
                            else if ((k<=15 && k>=8 ) &&j>7 ){m=5;}
                            else if ((k<=23 && k>=16) &&j>7 ){m=6;}
                            else if (k>=24 &&j>7 )         {m=7;}
                            else{m=4;}
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][0]),sizeof(uint8_t),1,new_image);
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][1]),sizeof(uint8_t),1,new_image);
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][2]),sizeof(uint8_t),1,new_image);
                            }
                    }
            }    
        }
    //case of h,v== 2,4
    else if(sof->sampling_horizontal[0]==2 && sof->sampling_vertical[0]==4){
        for (uint32_t l=0;l<MCU_vertical_number;l++){
        if (l==MCU_vertical_number-1){jmax=(sof->height)%(sof->sampling_vertical[0]*8);}
        else{jmax=sof->sampling_vertical[0]*8;}
            for (int j = 0; j < jmax; j++)
            for (uint32_t i=0;i<MCU_hori_number;i++)
                {if (i==MCU_hori_number-1){kmax=(sof->width)%(8*sof->sampling_horizontal[0]);}
                else{kmax=sof->sampling_horizontal[0]*8;}
                        for (int k = 0; k <kmax;k++){
                            /* m indicates the bloc
                            0 1 
                            2 3
                            4 5 
                            6 7
                            */
                            if      ((j<=15 && j>=8 ) &&k<=7 ){m=2;}
                            else if ((j<=23 && j>=16) &&k<=7 ){m=4;}
                            else if ((j>=24         ) &&k<=7 ){m=6;}
                            else if ((j<=7          ) &&k<=7 ){m=0;}
                            else if ((j<=15 && j>=8 ) &&k>7 ) {m=3;}
                            else if ((j<=23 && j>=16) &&k>7 ) {m=5;}
                            else if (j>=24 &&         k>7 ) {m=7;}
                            else{m=1;}
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][0]),sizeof(uint8_t),1,new_image);
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][1]),sizeof(uint8_t),1,new_image);
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][2]),sizeof(uint8_t),1,new_image);
                            }
                    }
            }    
        }
    //case of h,v == 3,2
    else if((sof->sampling_horizontal[0]==3 && sof->sampling_vertical[0]==2) ){

        for (uint32_t l=0;l<MCU_vertical_number;l++){
        if (l==MCU_vertical_number-1){jmax=(sof->height)%(sof->sampling_vertical[0]*8);}
        else{jmax=sof->sampling_vertical[0]*8;}
            for (int j = 0; j < jmax; j++)
            for (uint32_t i=0;i<MCU_hori_number;i++)
                {if (i==MCU_hori_number-1){kmax=(sof->width)%(8*sof->sampling_horizontal[0]);}
                else{kmax=sof->sampling_horizontal[0]*8;}
                        for (int k = 0; k <kmax;k++){
                            /* m indicates the bloc
                            0 1 2
                            3 4 5
                            */
                            if      ((k<=15 && k>=8 ) &&j<=7 ){m=1;}
                            else if ((k<=23 && k>=16) &&j<=7 ){m=2;}
                            else if ((k<=7          ) &&j<=7 ){m=0;}
                            else if ((k<=15 && k>=8 ) &&j>7 ){m=4;}
                            else if ((k<=23 && k>=16) &&j>7 ){m=5;}
                            else{m=3;}
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][0]),sizeof(uint8_t),1,new_image);
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][1]),sizeof(uint8_t),1,new_image);
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][2]),sizeof(uint8_t),1,new_image);
                            }
                    }
            }  
    }
    //case of h,v== 2,3
    else if((sof->sampling_horizontal[0]==2 && sof->sampling_vertical[0]==3) ){

        for (uint32_t l=0;l<MCU_vertical_number;l++){
        if (l==MCU_vertical_number-1){jmax=(sof->height)%(sof->sampling_vertical[0]*8);}
        else{jmax=sof->sampling_vertical[0]*8;}
            for (int j = 0; j < jmax; j++)
            for (uint32_t i=0;i<MCU_hori_number;i++)
                {if (i==MCU_hori_number-1){kmax=(sof->width)%(8*sof->sampling_horizontal[0]);}
                else{kmax=sof->sampling_horizontal[0]*8;}
                        for (int k = 0; k <kmax;k++){
                            /* m indicates the bloc
                            0 1 
                            2 3
                            4 5 
                            
                            */
                            if      ((j<=15 && j>=8 ) &&k<=7 ){m=2;}
                            else if ((j<=23 && j>=16) &&k<=7 ){m=4;}
                            else if ((j<=7          ) &&k<=7 ){m=0;}
                            else if ((j<=15 && j>=8 ) &&k>7 ) {m=3;}
                            else if ((j<=23 && j>=16) &&k>7 ) {m=5;}
                            else{m=1;}
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][0]),sizeof(uint8_t),1,new_image);
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][1]),sizeof(uint8_t),1,new_image);
                            fwrite(&(MCUs_RGB[i+l*(real_MCU_hori_number)][m*64+(j%8)*8+(k%8)][2]),sizeof(uint8_t),1,new_image);
                            }
                    }
            }   
    }
    else{
        printf("\n \n \n CASE NOT TREATED UP_SAPMLE: \n \n \n ");
        printf("%ux%u %ux%u %ux%u \n",sof->sampling_horizontal[0],sof->sampling_vertical[0],sof->sampling_horizontal[1],sof->sampling_vertical[1],sof->sampling_horizontal[2],sof->sampling_vertical[2]);        
        }
    fclose(new_image);
    free(new_file);
}


