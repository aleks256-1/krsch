#include <iostream>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <bitset>

#define SUBSTANCE_LENGTH 128
#define MAX_CREATURE_SIZE 512
#define N 2
#define MAX_GENOME_SIZE 32
#define MAX_COND_LENGTH 32
#define MAX_OPERON_LENGTH 32
#define MAX_COND_VALUE 127
#define MAX_OPERON_VALUE 127
using namespace std;


struct cell{
	unsigned int v[SUBSTANCE_LENGTH];
	int dv[SUBSTANCE_LENGTH];
};

//v[2], v[3], v[4] -- rgb

struct creature{
	int n;
	struct cell* cells;
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
};

void initCreature(struct creature ** creature){
	*creature = (struct creature*)malloc(sizeof(struct creature));
	(*creature)->n = N;
	(*creature)->cells = (struct cell*)calloc(N * N, sizeof(struct cell));
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			for(int k = 0; k < SUBSTANCE_LENGTH; k++){
				(*creature)->cells[i * N + j].v[k] = (*creature)->cells[i * N + j].dv[k] = 0;
			}
		}
	}
    (*creature)->cells[0].v[0] = 0;
    (*creature)->cells[0].v[1] = 0;
    (*creature)->cells[1].v[0] = 0;
    (*creature)->cells[1].v[1] = 1;
    (*creature)->cells[2].v[0] = 1;
    (*creature)->cells[2].v[1] = 0;
    (*creature)->cells[3].v[0] = 1;
    (*creature)->cells[3].v[1] = 1;
	
}

void initRandGenome(struct genome * genome){
	srand(time(NULL));
	genome->length = rand() % MAX_GENOME_SIZE;
	genome->genes = (struct gene*)calloc(genome->length, sizeof(struct gene));
	for(int i = 0; i < genome->length; i++){
		srand(i * genome->length);
		genome->genes[i].cond_length = rand() % MAX_COND_LENGTH;
		genome->genes[i].cond = (struct cond*)calloc(genome->genes[i].cond_length, sizeof(struct cond));
		for(int j = 0; j < genome->genes[i].cond_length; j++){
			srand(j * genome->length);
			genome->genes[i].cond[j].threshold = rand() % MAX_COND_VALUE;
			srand(j * genome->length + 1);
			genome->genes[i].cond[j].substance = rand() % MAX_COND_VALUE;
			srand(j * genome->length + 2);
			genome->genes[i].cond[j].sign = rand() % 2;
		}
		srand(i * 2 * genome->length);
		genome->genes[i].oper_length = rand() % MAX_OPERON_LENGTH;
		genome->genes[i].operons = (struct operon*)calloc(genome->genes[i].oper_length, sizeof(struct operon));
		for(int j = 0; j < genome->genes[i].oper_length; j++){
			srand(j * 2 * genome->length);
			genome->genes[i].operons[j].rate = rand() % MAX_OPERON_VALUE;
			srand(j * 2 * genome->length + 1);
			genome->genes[i].operons[j].substance = rand() % MAX_OPERON_VALUE;
			srand(j * 2 * genome->length + 2);
			genome->genes[i].operons[j].sign = rand() % 2;
		}
	}
}

void saveGenome(struct genome * genome){
	int i, j;
	for(i = 0; i < genome->length; i++){
		for(j = 0; j < genome->genes[i].cond_length; j++){
			uint16_t cond = 0;
			cond |= ((uint16_t)genome->genes[i].cond[j].sign) << 15;
			cond |= (uint16_t)genome->genes[i].cond[j].substance;
            cond |= ((uint16_t)genome->genes[i].cond[j].threshold) << 7;
            cond &= ~(1UL << 8);
            bitset<16> x(cond);
			cout << "cond = " << x << "\n";
		}
		for(j = 0; j < genome->genes[i].oper_length; j++){
			uint16_t oper = 0;
			oper |=	1 << 8;
			oper |= ((uint16_t)genome->genes[i].operons[j].sign) << 15;
			oper |= (uint16_t)genome->genes[i].operons[j].substance;
			oper |= ((uint16_t)genome->genes[i].operons[j].rate) << 7;
			bitset<16> x(oper);
			cout << "oper = " << x << "\n";
		}
	}
}

int main(int argc, char ** argv) {
    struct creature *creature;
    struct genome *genome;
    genome = (struct genome*)malloc(sizeof(struct genome));
    

    initCreature(&creature);
    initRandGenome(genome);
    saveGenome(genome);
}