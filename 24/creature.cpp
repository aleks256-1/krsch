#include <iostream>
#include "main.h"
#include "rdev.h"

using namespace std;

void printCreature(struct creature *creature) {
    int countOfCells = creature->n * creature->n;
    for (int i = 0; i < countOfCells; i++) {
        cout<<"cell #(v):"<<i<<":\n";
        for (int j = 0; j < SUBSTANCE_LENGTH; j++) {
            cout<<creature->cells[i].v[j] << " ";
        }
        cout<<"\n";
    }
}
void printCreature_dv(struct creature *creature) {
    int countOfCells = creature->n * creature->n;
    for (int i = 0; i < countOfCells; i++) {
        cout<<"cell #(dv):"<<i<<":\n";
        for (int j = 0; j < SUBSTANCE_LENGTH; j++) {
            cout<<creature->cells[i].dv[j] << " ";
        }
        cout<<"\n";
    }
}

void initCreature(struct creature ** creature){
	*creature = (struct creature*)malloc(sizeof(struct creature));
	(*creature)->n = N;
	(*creature)->cells = (struct cell*)calloc(N * N, sizeof(struct cell));
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			for(int k = 0; k < SUBSTANCE_LENGTH; k++){
				(*creature)->cells[i * N + j].v[k] = (*creature)->cells[i * N + j].dv[k] = RangedRandomNumber(0,255);
			}
		}
	}
}

void applyDvForCells(struct creature * creature) {
    int countOfCells = creature->n * creature->n;
    for(int i = 0; i < countOfCells; i++){
		for(int j = 0; j < SUBSTANCE_LENGTH; j++){
            int temp = creature->cells[i].v[j];
				if(temp + creature->cells[i].dv[j] < 0){
					creature->cells[i].v[j] = 0;
					continue;
				}
				else if(temp + creature->cells[i].dv[j] > 255){
					creature->cells[i].v[j] = 255;
					continue;
				}
				creature->cells[i].v[j] += creature->cells[i].dv[j];
        }
    }
}

void applyDiff(struct creature * creature){
	for(int i = 0; i < creature->n; i++){
		for(int j = 0; j < creature->n; j++){
			for(int k = 0; k < SUBSTANCE_LENGTH; k++){
				if(creature->cells[i * creature->n + j].dv[k] > 255){
					creature->cells[i * creature->n + j].v[k] = 255;
					continue;
				}
				else if(creature->cells[i * creature->n + j].dv[k] < 0){
					creature->cells[i * creature->n + j].v[k] = 0;
					continue;
				}
				creature->cells[i * creature->n + j].v[k] = creature->cells[i * creature->n + j].dv[k];
			}
		}
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