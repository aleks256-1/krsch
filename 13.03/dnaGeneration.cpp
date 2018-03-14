#include <iostream>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <bitset>
#include <math.h>
#include <stdio.h>
#include <stdint.h>


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
	float v[SUBSTANCE_LENGTH];
	float dv[SUBSTANCE_LENGTH];
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





float calc_sigma(float x){
	return 2.164 * (1 / (1 + exp(-x))) - 0.581989;
}


void initCreature(struct creature ** creature){
	*creature = (struct creature*)malloc(sizeof(struct creature));
	(*creature)->n = N;
	(*creature)->cells = (struct cell*)calloc(N * N, sizeof(struct cell));
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			for(int k = 0; k < SUBSTANCE_LENGTH; k++){
				(*creature)->cells[i * N + j].v[k] = (*creature)->cells[i * N + j].dv[k] = 255;
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

void calculateDvForCells(struct creature * creature, struct genome * genome) {
    int countOfCells = creature->n * creature->n;
    for(int i = 0; i < countOfCells; i++){
		for(int j = 0; j < SUBSTANCE_LENGTH; j++){
			creature->cells[i].dv[j] = 0;
		}
	}
    for (int i = 0; i < countOfCells; i++) {
        for (int j = 0; j < genome->genes->cond_length; j++) {
//1 == ">"
            int delta;
            if (genome->genes->cond[i].sign == 1) {
                delta = genome->genes->cond[j].substance - genome->genes->cond[j].threshold;
            } else {
                delta = genome->genes->cond[j].threshold - genome->genes->cond[j].substance;
            }
            for (int k = 0; k < genome->genes->oper_length; k++) {
//1 == ">"
                if (genome->genes->operons[k].sign == 1) {
                    creature->cells[i].dv[genome->genes->operons[k].substance] += genome->genes->operons[k].rate * calc_sigma(delta);
                } else {
                    creature->cells[i].dv[genome->genes->operons[k].substance] -= genome->genes->operons[k].rate * calc_sigma(delta);
                }
                if (creature->cells[i].dv[genome->genes->operons[k].substance] > 255) {
                    creature->cells[i].dv[genome->genes->operons[k].substance] = 255;
                }
                if (creature->cells[i].dv[genome->genes->operons[k].substance] < 0) {
                    creature->cells[i].dv[genome->genes->operons[k].substance] = 0;
                }
            }
        }
    }
/*    for (int i = 0; i < countOfCells; i++) {
        for (int j = 0; j < SUBSTANCE_LENGTH; j++) {
            cout << creature->cells[i].dv[j] << " ";
        }
        cout << "\n";
    }
*/
}

void applyDvForCells(struct creature * creature) {
    int countOfCells = creature->n * creature->n;
    for(int i = 0; i < countOfCells; i++){
		for(int j = 0; j < SUBSTANCE_LENGTH; j++){
            creature->cells[i].v[j] += creature->cells[i].dv[j];
            creature->cells[i].dv[j] = 0;
            if (creature->cells[i].v[j] > 255) {
                    creature->cells[i].v[j] = 255;
            }
            if (creature->cells[i].v[j] < 0) {
                creature->cells[i].v[j] = 0;
		    }
        }
    }
}

void diff(struct creature * creature) {
    int n = creature->n;
    for (int i = 0; i < creature->n; i++) {
        for (int j = 0; j < creature->n; j++) {
            for (int k = 0; k < SUBSTANCE_LENGTH; k++) {
                if (int delta = j+1 < creature->n) {
                    creature->cells[i * n + delta].v[k] += 0.05 * creature->cells[i * n + j].v[k];
                }
                if (int delta = j-1 >= 0) {
                    creature->cells[i * n + delta].v[k] += 0.05 * creature->cells[i * n + j].v[k];
                }
                if (int delta = i+1 < creature->n) {
                    creature->cells[delta * n + j].v[k] += 0.05 * creature->cells[i * n + j].v[k];
                    if (int delta2 = j+1 < creature->n) {
                        creature->cells[delta * n + delta2].v[k] += 0.025 * creature->cells[i * n + j].v[k];
                    }
                    if (int delta2 = j-1 >= 0) {
                        creature->cells[delta * n + delta2].v[k] += 0.025 * creature->cells[i * n + j].v[k];
                    }
                }
                if (int delta = i-1 >=0) {
                    creature->cells[delta * n + j].v[k] += 0.05 * creature->cells[i * n + j].v[k];
                    if (int delta2 = j+1 < creature->n) {
                        creature->cells[delta * n + delta2].v[k] += 0.025 * creature->cells[i * n + j].v[k];
                    }
                    if (int delta2 = j-1 >= 0) {
                        creature->cells[delta * n + delta2].v[k] += 0.025 * creature->cells[i * n + j].v[k];
                    }
                }
                creature->cells[i * n + j].v[k] *= 0.7;
            }
        } 
    }
}

void printCreature(struct creature *creature) {
    int countOfCells = creature->n * creature->n;
    for (int i = 0; i < countOfCells; i++) {
        cout<<"cell #"<<i<<":\n";
        for (int j = 0; j < SUBSTANCE_LENGTH; j++) {
            cout<<creature->cells[i].v[j] << " ";
        }
        cout<<"\n";
    }
}

struct creature* grow(struct creature * creature){
	struct creature *new_creature = (struct creature*)malloc(sizeof(struct creature));
	int new_size = 2 * creature->n;
	new_creature->n = new_size;
	new_creature->cells = (struct cell*)calloc(new_creature->n * new_creature->n, sizeof(struct cell));
	for(int i = 0; i < new_size; i++){
		for(int j = 0; j < new_size; j++){
			for(int k = 0; k < SUBSTANCE_LENGTH; k++){
				new_creature->cells[i * new_size + j].v[k] = creature->cells[(i/2) * creature->n + j/2].v[k];
				new_creature->cells[i * new_size + j].dv[k] = creature->cells[(i/2) * creature->n + j/2].dv[k];
			}
		}
	}
	free(creature);
	return new_creature;
}


int main(int argc, char ** argv) {
    struct creature *creature;
    struct genome *genome;
    genome = (struct genome*)malloc(sizeof(struct genome));
    

    initCreature(&creature);
    initRandGenome(genome);
 //   saveGenome(genome);

    int step = 0;
    while(creature->n < MAX_CREATURE_SIZE) {
        calculateDvForCells(creature, genome);
        applyDvForCells(creature);
        diff(creature); 
        if (step % 2 == 0) {
            creature = grow(creature);
        }
        step++;
    }
    printCreature(creature);
    free(genome);
    free(creature);
}
