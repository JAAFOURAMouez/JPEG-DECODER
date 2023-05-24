<div align="center">
  <h1><span style="color:blue">OUR JFIF/JPEG DECODER</span></h1>
</div>

## <span style="color:red">MODULES AND SPECIFICATIONS: </span>

- MODE BASELINE:
  - PROGRAM ENTRY: IMAGE FILE JPEG/JPG  -> OUT FILE: PPM IMAGE OR PGM IMAGE
  - Executable name: `jpeg2ppm`

### <span style="color:red">File Structure: </span>

- `jpeg_reader.c`
  - JPEG head extraction: `HEADER structure`
    - Image size extraction (SOF) -> height x width
    - Sampling factor extraction (SOF) -> in [1..4]
    - Components number N (SOF) -> N between 1 and 3
    - Quantification table extraction (DQT) -> precision, i_Q, table values
    - Huffman tables (DHT) -> symbols number by length (1 to 16 bits) and symbols and huffman trees 
    - Start of Scan (SOS) : components_indexes i_c and DQT tables associated 
- `bitstream.c`
  - This module goes through the image bitstream needed to decode,eliminates the byte stuffing , extracts it in a bitstream.txt file and then stores 
 it in a string (char*) (Not the most optimal .. )
 
- `bitstream_to_mcu.c` / `extraction_mcu.c`  
  - The module `bitstream_to_mcu.c` extracts all the MCUs from the jpeg image and stores them in a new structure `LMCU` which contains a list of type `STRUCTURE MCU` and the MCU structure contains LY , LCb, LCr components which are lists of vectorial blocks ( LMCU->MCUs[i]->LY[i]->content ).
  this module uses the submodule `extraction_mcu.c` which helps by extracting the blocks order (in SOS) and the number of blocks in every component and stores them in a list which will be used in `bitstream_to_mcu.c`. 

- `Huffmann.c` 
  - This module extracts huffmann trees containing paths for each symbol in the DHT table extracted in the `HEADER structure` and stores them in a `huffnode structure`
  
- `treatment.c` and `izz.c` 
  - These two modules perform the Inverse quantification and inverse zigzag on all vectorial MCUs and return the new Matricial MCUs in a new structure called `M_LMCU` that is similar to `LMCU` but its components' blocks are matricial of type (int16_t) ,not vectorial and are izzed and iqed . 
  
- `treatment.c` and `idct.c` (fast or slow)
  - These two modules perform the Inverse Discrete cosine transformation on all matricial MCUs(of type int16_t) and return the new Matricial MCUs of type (uint8_t) in a new structure called `iM_LMCU` that is similar to `M_LMCU` but its components' blocks are matricial of type (uint8_t) ,not of type (int16_t) , not vectorial and are idct'ed.
  
- `upsample.c` 
  - The upsampling module works on the `iM_LMCU` structure coming from IDCT and expands the size of LCb and LCr blocks to the size of LY blocks . Then comes the duplication of rows or columns according to the sampling factors . Our decoder works on all sampling types according to the JPEG norm .
  
  
- `to_RGB.c`
  - This module converts the `iM_LMCU` that has been extracted after IDCT and converts it to RGB matrix or vector (if black and white image). The vector or matrix is `MCUs_RGB` and is used after this step to create the PGM/PPM image.
  
- `Decoded_image_creator.c`
  - This module writes the PPM/PGM image accordingly to the PPM/PGM norm . Our decoder works on cases having truncating problems and on all possible sampling situations and writes the RGB content in the image content . 

- `main.c`
  - Read image ".jpeg" or ".jpg"
  - Call `jpeg_reader`
    - ...treatment...
  - Close image
  - Call `decoded_image_creator`

![$$\color{blue}{JPEG\ DECODER\ STRUCTURE\\}$$](projet_C.jpeg)
