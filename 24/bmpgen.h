#ifndef BMPGEN_H
#define BMPGEN_H

#define fileHeaderSize 14
#define infoHeaderSize 40
#define bytesPerPixel 3

unsigned char* createBitmapFileHeader(int height, int width);
unsigned char* createBitmapInfoHeader(int height, int width);
void generateBitmapImage(unsigned char *image, int height, int width, char* imageFileName);
void createImage(struct creature * creature, char* imageFileName);
void readBitmapImage(unsigned char *image, int height, int width, char * imageFileName);
#endif