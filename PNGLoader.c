#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "PNGLoader.h"

GLuint loadPNG(char *filename) {
  /*
  FILE *file = fopen(filename, "r");
  
  // Check the first 8 bytes match the PNG signature
  uint8_t desiredSignature[] = { 137, 80, 78, 71, 13, 10, 26, 10 };
  uint8_t actualSignature[8];
  if (fread(signature, 1, 8, file) != 8 ||
      !memcmp(actualSignature, desiredSignature, 8)) {
    printf("Invalid file signature, this is not a PNG file.\n");
    return 0;
  }
  
  // these are required chunks, the rest are ancillary
  uint8_t foundIHDR, foundIEND, foundIDAT = 0;
  
  // image info, to be filled in later
  uint32_t width, height
  uint8_t depth, colorType, compressionMethod, filterMethod, interlaceMethod;
  
  // we might have a palette, if so it will be stored here
  int numPaletteEntries = 0;
  uint8_t *palette;
  
  // where the actual image data will end up
  uint8_t **image;
  
  while (!foundIEND) {
    uint32_t length, crc;
    uint8_t chunkType[4];
    uint8_t *data;
    
    // read the length of the chunk
    if (fread(&length, 4, 1, file) != 4) {
      printf("Unexpected EOF while reading chunk length.\n");
      return 0;
    }
    
    // read the chunk type
    if (fread(&chunkType, 1, 4, file) != 4) {
      printf("Unexpected EOF while reading chunk type.\n");
      return 0;
    }
    
    // read the chunk data
    data = (uint8_t *) malloc(length);
    if (fread(data, 1, length, file) != length) {
      printf("Unexpected EOF while reading chunk data.\n");
      return 0;
    }
    
    // read the crc
    if (fread(&crc, 4, 1, file) != 4) {
      printf("Unexpected EOF while reading crc.\n");
      return 0;
    }
    
    // now check what type of chunk it actually is
    if (memcmp(chunkType, "IHDR", 4)) {
      if (sscanf(chunkType, "%4d%4d%1d%1d%1d%1d%1d",
	  &width, &height, &depth, &colorType, &compressionMethod,
	  &filterMethod, &interlaceMethod) != 7) {
	printf("Invalid IHDR data");
	return 0:
      }
      foundIHDR = 1;
      
      if (colorType != 2) {
	printf("Invalid color type, only supports RGB");
	return 0;
      }
    }
    else if (memcmp(chunkType, "IEND", 4)) {
      foundIEND = 1;
    }
    else if (memcmp(chunkType, "PLTE", 4)) {
      
    }
  }
  */
  return 0;
}