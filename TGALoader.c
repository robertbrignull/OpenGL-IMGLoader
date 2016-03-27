#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "TGALoader.h"

GLuint loadTGA(char *filename) {
  FILE *file = fopen(filename, "r");
  
  if (file == NULL) {
    printf("Error openning file, '%s'.\n", filename);
    return 0;
  }
  
  // read the file header
  uint8_t  IDLength, colorMapType, imageType, colorMapEntrySize, depth, imageDescriptor;
  uint16_t firstEntryIndex, colorMapLength, XOrigin, YOrigin, width, height;
  
  fread(&IDLength, 1, 1, file);
  fread(&colorMapType, 1, 1, file);
  fread(&imageType, 1, 1, file);
  fread(&firstEntryIndex, 2, 1, file);
  fread(&colorMapLength, 2, 1, file);
  fread(&colorMapEntrySize, 1, 1, file);
  fread(&XOrigin, 2, 1, file);
  fread(&YOrigin, 2, 1, file);
  fread(&width, 2, 1, file);
  fread(&height, 2, 1, file);
  fread(&depth, 1, 1, file);
  fread(&imageDescriptor, 1, 1, file);
  
  // check all the above read properly
  if (ftell(file) != 18) {
    printf("Error while reading file header.\n");
    return 0;
  }
  
  /*
  printf("IDLength = %u\n", IDLength);
  printf("colorMapType = %u\n", colorMapType);
  printf("imageType = %u\n", imageType);
  printf("firstEntryIndex = %u\n", firstEntryIndex);
  printf("colorMapLength = %u\n", colorMapLength);
  printf("colorMapEntrySize = %u\n", colorMapEntrySize);
  printf("XOrigin = %u\n", XOrigin);
  printf("YOrigin = %u\n", YOrigin);
  printf("width = %u\n", width);
  printf("height = %u\n", height);
  printf("depth = %u\n", depth);
  printf("imageDescriptor = %u\n", imageDescriptor);
  */
  
  // read the image ID section
  uint8_t *imageID = (uint8_t*) malloc(IDLength);
  fread(imageID, 1, IDLength, file);
  
  // allocate space for the image data
  // always use 32bit RGBA internally
  uint8_t *imageData = (uint8_t*) malloc(width * height * 4);
  
  // now load the image data, depending on the encoding type
  if (imageType == 1) { // non-RLE colormap
    uint8_t *colorMap = (uint8_t*) malloc(colorMapLength * 4);
    fseek(file, firstEntryIndex, SEEK_CUR);
    if (colorMapEntrySize == 15 || colorMapEntrySize == 16) {
      printf("Pixel depth %d unsupported.\n", colorMapEntrySize);
      return 0;
    }
    else if (colorMapEntrySize == 24) {
      int i;
      for (i = 0; i < colorMapLength; ++i) {
	fread(&colorMap[4*i+2], 1, 1, file);
	fread(&colorMap[4*i+1], 1, 1, file);
	fread(&colorMap[4*i+0], 1, 1, file);
	colorMap[4*i+3] = 255;
      }
    }
    else if (colorMapEntrySize == 32) {
      int i;
      for (i = 0; i < colorMapLength; ++i) {
	fread(&colorMap[4*i+2], 1, 1, file);
	fread(&colorMap[4*i+1], 1, 1, file);
	fread(&colorMap[4*i+0], 1, 1, file);
	fread(&colorMap[4*i+3], 1, 1, file);
      }
    }
    else {
      printf("Unknown pixel depth value, %d.\n", colorMapEntrySize);
      return 0;
    }
    
    if (depth == 8) {
      uint8_t index;
      int i;
      for (i = 0; i < width * height; ++i) {
	fread(&index, 1, 1, file);
	if (index >= colorMapLength) {
	  printf("Invalid color map index.\n");
	  return 0;
	}
	memcpy(&imageData[4*i], &colorMap[4*index], 4);
      }
    }
    else if (depth == 16) {
      uint16_t index;
      int i;
      for (i = 0; i < width * height; ++i) {
	fread(&index, 2, 1, file);
	if (index >= colorMapLength) {
	  printf("Invalid color map index.\n");
	  return 0;
	}
	memcpy(&imageData[4*i], &colorMap[4*index], 4);
      }
    }
    else {
      printf("Unknown index depth value, %d.\n", depth);
    }
    
    free(colorMap);
  }
  else if (imageType == 2) { // non-RLE true-colour
    // skip the colormap data
    if (colorMapType != 0) {
      fseek(file, firstEntryIndex + (colorMapLength * colorMapEntrySize), SEEK_CUR);
    }
    
    if (depth == 15 || depth == 16) {
      printf("Pixel depth %d unsupported.\n", depth);
      return 0;
    }
    else if (depth == 24) {
      int i;
      for (i = 0; i < width * height; ++i) {
	fread(&imageData[4*i+2], 1, 1, file);
	fread(&imageData[4*i+1], 1, 1, file);
	fread(&imageData[4*i+0], 1, 1, file);
	imageData[4*i+3] = 255;
      }
    }
    else if (depth == 32) {
      int i;
      for (i = 0; i < width * height; ++i) {
	fread(&imageData[4*i+2], 1, 1, file);
	fread(&imageData[4*i+1], 1, 1, file);
	fread(&imageData[4*i+0], 1, 1, file);
	fread(&imageData[4*i+3], 1, 1, file);
      }
    }
    else {
      printf("Unknown pixel depth value, %d.\n", depth);
      return 0;
    }
  }
  else if (imageType == 3) { // non-RLE greyscale
    // skip the colormap data
    if (colorMapType != 0) {
      fseek(file, firstEntryIndex + (colorMapLength * colorMapEntrySize), SEEK_CUR);
    }
    
    if (depth == 8) {
      uint8_t value;
      int i;
      for (i = 0; i < width * height; ++i) {
	fread(&value, 1, 1, file);
	imageData[4*i+0] = value;
	imageData[4*i+1] = value;
	imageData[4*i+2] = value;
	imageData[4*i+3] = 255;
      }
    }
    else {
      printf("Pixel depth %d unsupported for greyscale images.\n", depth);
      return 0;
    }
  }
  else if (imageType == 9) { // RLE colormap
    uint8_t *colorMap = (uint8_t*) malloc(colorMapLength * 4);
    fseek(file, firstEntryIndex, SEEK_CUR);
    if (colorMapEntrySize == 15 || colorMapEntrySize == 16) {
      printf("Pixel depth %d unsupported.\n", colorMapEntrySize);
      return 0;
    }
    else if (colorMapEntrySize == 24) {
      int i;
      for (i = 0; i < colorMapLength; ++i) {
	fread(&colorMap[4*i+2], 1, 1, file);
	fread(&colorMap[4*i+1], 1, 1, file);
	fread(&colorMap[4*i+0], 1, 1, file);
	colorMap[4*i+3] = 255;
      }
    }
    else if (colorMapEntrySize == 32) {
      int i;
      for (i = 0; i < colorMapLength; ++i) {
	fread(&colorMap[4*i+2], 1, 1, file);
	fread(&colorMap[4*i+1], 1, 1, file);
	fread(&colorMap[4*i+0], 1, 1, file);
	fread(&colorMap[4*i+3], 1, 1, file);
      }
    }
    else {
      printf("Unknown pixel depth value, %d.\n", colorMapEntrySize);
      return 0;
    }
    
    if (depth == 8) {
      uint8_t index;
      int i = 0;
      while (i < width * height) {
	uint8_t header;
	fread(&header, 1, 1, file);
	if (header > 127) { // run-length packet
	  header &= 0x7F;
	  fread(&index, 1, 1, file);
	  if (index >= colorMapLength) {
	    printf("Invalid color map index.\n");
	    return 0;
	  }
	  int k;
	  for (k = 0; k <= header; ++k) {
	    memcpy(&imageData[4*i], &colorMap[4*index], 4);
	    i += 1;
	  }
	}
	else { // raw packet
	  int k;
	  for (k = 0; k <= header; ++k) {
	    fread(&index, 1, 1, file);
	    if (index >= colorMapLength) {
	      printf("Invalid color map index.\n");
	      return 0;
	    }
	    memcpy(&imageData[4*i], &colorMap[4*index], 4);
	    i += 1;
	  }
	}
      }
    }
    else if (depth == 16) {
      uint16_t index;
      int i = 0;
      while (i < width * height) {
	uint8_t header;
	fread(&header, 1, 1, file);
	if (header > 127) { // run-length packet
	  header &= 0x7F;
	  fread(&index, 2, 1, file);
	  if (index >= colorMapLength) {
	    printf("Invalid color map index.\n");
	    return 0;
	  }
	  int k;
	  for (k = 0; k <= header; ++k) {
	    memcpy(&imageData[4*i], &colorMap[4*index], 4);
	    i += 1;
	  }
	}
	else { // raw packet
	  int k;
	  for (k = 0; k <= header; ++k) {
	    fread(&index, 2, 1, file);
	    if (index >= colorMapLength) {
	      printf("Invalid color map index.\n");
	      return 0;
	    }
	    memcpy(&imageData[4*i], &colorMap[4*index], 4);
	    i += 1;
	  }
	}
      }
    }
    else {
      printf("Unknown index depth value, %d.\n", depth);
    }
    
    free(colorMap);
  }
  else if (imageType == 10) { // RLE true-color
    // skip the colormap data
    if (colorMapType != 0) {
      fseek(file, firstEntryIndex + (colorMapLength * colorMapEntrySize), SEEK_CUR);
    }
    
    if (depth == 15 || depth == 16) {
      printf("Pixel depth %d unsupported.\n", depth);
      return 0;
    }
    else if (depth == 24) {
      int i = 0;
      while (i < width * height) {
	uint8_t header;
	fread(&header, 1, 1, file);
	if (header > 127) { // run-length packet
	  header &= 0x7F;
	  uint8_t color[3];
	  int k;
	  
	  fread(&color[2], 1, 1, file);
	  fread(&color[1], 1, 1, file);
	  fread(&color[0], 1, 1, file);
	  
	  for (k = 0; k <= header; ++k) {
	    imageData[4*i+0] = color[0];
	    imageData[4*i+1] = color[1];
	    imageData[4*i+2] = color[2];
	    imageData[4*i+3] = 255;
	    i += 1;
	  }
	}
	else { // raw packet
	  int k = 0;
	  for (k = 0; k <= header; ++k) {
	    fread(&imageData[4*i+2], 1, 1, file);
	    fread(&imageData[4*i+1], 1, 1, file);
	    fread(&imageData[4*i+0], 1, 1, file);
	    imageData[4*i+3] = 255;
	    i += 1;
	  }
	}
      }
    }
    else if (depth == 32) {
      int i = 0;
      while (i < width * height) {
	uint8_t header;
	fread(&header, 1, 1, file);
	if (header > 127) { // run-length packet
	  header &= 0x7F;
	  uint8_t color[4];
	  int k;
	  
	  fread(&color[2], 1, 1, file);
	  fread(&color[1], 1, 1, file);
	  fread(&color[0], 1, 1, file);
	  fread(&color[3], 1, 1, file);
	  
	  for (k = 0; k < header; ++k) {
	    imageData[4*i+0] = color[0];
	    imageData[4*i+1] = color[1];
	    imageData[4*i+2] = color[2];
	    imageData[4*i+3] = color[3];
	    i += 1;
	  }
	}
	else { // raw packet
	  int k = 0;
	  for (k = 0; k < header; ++k) {
	    fread(&imageData[4*i+2], 1, 1, file);
	    fread(&imageData[4*i+1], 1, 1, file);
	    fread(&imageData[4*i+0], 1, 1, file);
	    fread(&imageData[4*i+3], 1, 1, file);
	    i += 1;
	  }
	}
      }
    }
    else {
      printf("Unknown index depth value, %d.\n", depth);
    }
  }
  else if (imageType == 11) { // RLE greyscale
    // skip the colormap data
    if (colorMapType != 0) {
      fseek(file, firstEntryIndex + (colorMapLength * colorMapEntrySize), SEEK_CUR);
    }
    
    if (depth == 8) {
      uint8_t value;
      int i = 0;
      while (i < width * height) {
	uint8_t header;
	fread(&header, 1, 1, file);
	if (header > 127) { // run-length packet
	  header &= 0x7F;
	  fread(&value, 1, 1, file);
	  int k;
	  for (k = 0; k < header; ++k) {
	    imageData[4*i+0] = value;
	    imageData[4*i+1] = value;
	    imageData[4*i+2] = value;
	    imageData[4*i+3] = 255;
	    i += 1;
	  }
	}
	else { // raw packet
	  int k = 0;
	  for (k = 0; k < header; ++k) {
	    fread(&value, 1, 1, file);
	    imageData[4*i+0] = value;
	    imageData[4*i+1] = value;
	    imageData[4*i+2] = value;
	    imageData[4*i+3] = 255;
	    i += 1;
	  }
	}
      }
    }
    else {
      printf("Pixel depth %d unsupported for greyscale images.\n", depth);
      return 0;
    }
  }
  else {
    printf("Unsupported image type.\n");
    return 0;
  }
  
  /*
  printf("data = ");
  int i;
  for (i = 0; i < width * height; ++i) {
    printf("%02X%02X%02X%02X ", imageData[4*i+0], imageData[4*i+1], imageData[4*i+2], imageData[4*i+3]);
  }
  printf("\n");
  */
  
  fclose(file);
  
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
  
  free(imageID);
  free(imageData);
  
  return texture;
}