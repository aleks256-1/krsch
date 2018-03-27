#ifndef CREATURE_H
#define CREATURE_H

#define SUBSTANCE_LENGTH 128
#define MAX_CREATURE_SIZE 256
#define RED_COMPONENT 4
#define GREEN_COMPONENT 3
#define BLUE_COMPONENT 2

struct cell{
	unsigned int v[SUBSTANCE_LENGTH];
	int dv[SUBSTANCE_LENGTH];
};

//v[2], v[3], v[4] -- bgr

struct creature{
	int n;
	struct cell* cells;
};

void initCreature(struct creature * creature);
struct creature* grow(struct creature * creature);
void applyDvForCells(struct creature * creature);
void applyDiff(struct creature * creature);
void printCreature(struct creature *creature);
void printCreature_dv(struct creature *creature);

#endif