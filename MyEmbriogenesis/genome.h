#ifndef GENOME_H
#define GENOME_H

#define MAX_GENOME_SIZE 32
#define MAX_COND_LENGTH 32
#define MAX_OPERON_LENGTH 32
#define MAX_COND_VALUE 127
#define MAX_OPERON_VALUE 127

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
};

void loadGenome(struct genome * genome, const char * path);
void initRandGenome(struct genome * genome);
void saveGenome(struct genome * genome, const char * path);

#endif
