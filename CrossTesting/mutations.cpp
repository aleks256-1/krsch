#include <iostream>
#include <vector>
#include "rdev.h"
#include "genome.h"

void RandomFragmentDeletion(struct genome * genome) {
    saveGenome(genome);
    FILE * fp;
    fp = fopen("genome.txt", "rb");
    fseek(fp,0,SEEK_END);
    int size = (ftell(fp))/2;
	rewind(fp);
	uint16_t* buffer = (uint16_t*)malloc(size * sizeof(uint16_t));
    fread(buffer, sizeof(uint16_t), size, fp);
    fclose(fp);

    int remove = RangedRandomNumber(0, size);
    int fragmentsToRemove = RangedRandomNumber(0, size - remove);
    int newSize = size - fragmentsToRemove;
    uint16_t* newbuffer = (uint16_t*)malloc(newSize * sizeof(uint16_t));

    for (int i = 0; i < remove; i++) {
        newbuffer[i] = buffer[i];
    }
    for (int i = remove + fragmentsToRemove, j = remove; i < size; i++, j++) {
        newbuffer[j] = buffer[i];
    }
    fp = fopen("genome.txt", "wb");
    for(int i = 0; i < newSize; i++) {
        fwrite(&newbuffer[i], sizeof(uint16_t), 1, fp);
    }
    fclose(fp);
    loadGenome(genome);
    free(buffer);
    free(newbuffer);
}

void ChangeRandomBits(struct genome * genome) {
    saveGenome(genome);
    FILE * fp;
    fp = fopen("genome.txt", "rb");
    fseek(fp,0,SEEK_END);
    int size = (ftell(fp))/2;
	rewind(fp);
	uint16_t* buffer = (uint16_t*)malloc(size * sizeof(uint16_t));
    fread(buffer, sizeof(uint16_t), size, fp);
    fclose(fp);

    for (int i = 0; i < size; i++) {
        uint16_t uint16 = 1;
        if (RangedRandomNumber(0,100) > 25) {
            buffer[i] ^= (uint16 << RangedRandomNumber(0,15));
        }
    }
    fp = fopen("genome.txt", "wb");
    for(int i = 0; i < size; i++) {
        fwrite(&buffer[i], sizeof(uint16_t), 1, fp);
    }
    fclose(fp);
    loadGenome(genome);
    free(buffer);
 }