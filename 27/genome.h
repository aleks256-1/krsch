#ifndef GENOME_H
#define GENOME_H
#include <stdint.h>

#define MIN_GENOME_SIZE 15
#define MAX_GENOME_SIZE 32
#define MAX_COND_LENGTH 32
#define MAX_OPERON_LENGTH 32
#define MAX_COND_VALUE 127
#define MAX_OPERON_VALUE 127
#define MAX_UINT16 65535

using namespace std;

struct uint16_genome {
    uint16_t * genes;
    int size;
    float similarity;
};

struct operon{
    unsigned char rate : 7;
	unsigned char sign : 1;
    unsigned char substance : 7;
};

struct cond{
    unsigned char threshold : 7;
    unsigned char sign : 1;
    unsigned char substance : 7;
};

struct gene{
    struct cond *cond;
    struct operon *operons;
    int cond_length;
	int oper_length;
};

struct genome{
    struct gene *genes;
    int length;
    float similarity;
};

void copyGenome_uint16(uint16_genome * genome1, uint16_genome * genome2);
void Print_uint16_genome(struct uint16_genome * genome);
void initRandGenome_uint16(uint16_genome * genome);
void printGenome(struct genome * genome);
void loadGenome(struct genome * genome);
void initRandGenome(struct genome * genome);
void saveGenome(struct genome * genome);
void copyGenome(struct genome * genome1, struct genome * genome2);
void uint16genome_to_genome(struct uint16_genome * genome1, struct genome * genome2);
void genome_to_uint16genome(struct uint16_genome * genome1, struct genome * genome2);

#endif
