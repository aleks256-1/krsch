#include <iostream>
#include <stdio.h>
#include <fstream>
#include "creature.h"
#include "bmpgen.h"

using namespace std;

unsigned char* createBitmapFileHeader(int height, int width){
    int fileSize = fileHeaderSize + infoHeaderSize + bytesPerPixel*height*width;

    static unsigned char fileHeader[] = {
        0,0, /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };
    fileHeader[ 0] = (unsigned char)('B');
    fileHeader[ 1] = (unsigned char)('M');
    fileHeader[ 2] = (unsigned char)(fileSize    );
    fileHeader[ 3] = (unsigned char)(fileSize>> 8);
    fileHeader[ 4] = (unsigned char)(fileSize>>16);
    fileHeader[ 5] = (unsigned char)(fileSize>>24);
    fileHeader[10] = (unsigned char)(fileHeaderSize + infoHeaderSize);

    return fileHeader;
}

unsigned char* createBitmapInfoHeader(int height, int width){
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0, /// number of color planes
        0,0, /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };
    infoHeader[ 0] = (unsigned char)(infoHeaderSize);
    infoHeader[ 4] = (unsigned char)(width    );
    infoHeader[ 5] = (unsigned char)(width>> 8);
    infoHeader[ 6] = (unsigned char)(width>>16);
    infoHeader[ 7] = (unsigned char)(width>>24);
    infoHeader[ 8] = (unsigned char)(height    );
    infoHeader[ 9] = (unsigned char)(height>> 8);
    infoHeader[10] = (unsigned char)(height>>16);
    infoHeader[11] = (unsigned char)(height>>24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(bytesPerPixel*8);

    return infoHeader;
}

void generateBitmapImage(unsigned char *image, int height, int width, char* imageFileName){
    unsigned char* fileHeader = createBitmapFileHeader(height, width);
    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4-(width*bytesPerPixel)%4)%4;
    FILE* imageFile = fopen(imageFileName, "wb");
    fwrite(fileHeader, 1, fileHeaderSize, imageFile);
    fwrite(infoHeader, 1, infoHeaderSize, imageFile);
    int i;
    for(i=0; i<height; i++){
        fwrite(image+(i*width*bytesPerPixel), bytesPerPixel, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }
    fclose(imageFile);
}

void createImage(struct creature * creature, char* imageFileName) {
    unsigned char image[creature->n][creature->n][bytesPerPixel];
    for(int i = 0; i < creature->n; i++){
        for(int j = 0; j < creature->n; j++){
            image[i][j][2] = (unsigned char)(creature->cells[i * creature->n + j].v[RED_COMPONENT]); ///red
            image[i][j][1] = (unsigned char)(creature->cells[i * creature->n + j].v[GREEN_COMPONENT]); ///green
            image[i][j][0] = (unsigned char)(creature->cells[i * creature->n + j].v[BLUE_COMPONENT]); ///blue
        }
    }
    generateBitmapImage((unsigned char *)image, creature->n, creature->n, imageFileName);
}

void readBitmapImage(unsigned char *image, int height, int width, char * imageFileName) {
    unsigned char* fileHeader = createBitmapFileHeader(height, width);
    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4-(width*bytesPerPixel)%4)%4;
    FILE* imageFile = fopen(imageFileName, "rb");
    fread(fileHeader, 1, fileHeaderSize, imageFile);
    fread(infoHeader, 1, infoHeaderSize, imageFile);
    int i;
    for(i=0; i<height; i++){
        fread(image+(i*width*bytesPerPixel), bytesPerPixel, width, imageFile);
        fread(padding, 1, paddingSize, imageFile);
    }
    fclose(imageFile);
}
