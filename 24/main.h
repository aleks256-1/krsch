#ifndef MAIN_H
#define MAIN_H
#include "creature.h"

#define SUBSTANCE_LENGTH 128
#define N 4
#define GROW_SIZE 2
#define START_POPULATION 5
#define IDEAL_CREATURE 5
#define NEXT_GENERATION_POPULATION 10

struct matrix{
	int size;
	float *val;
	float norm_rate;
};

void init_blur_matrix(struct matrix ** matrix);

#endif