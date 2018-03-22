#include "creature.h"
#include "genome.h"
#include "main.h"
#include "math.h"
#include <iostream>

using namespace std;

float calc_sigma(float x){
	return 2.164 * (1 / (1 + exp(-x))) - 0.581989;
}

void calculateDvForCells_v2(struct creature * creature, struct genome * genome) {
    int countOfCells = creature->n * creature->n;
    for(int i = 0; i < countOfCells; i++){
		for(int k = 0; k < SUBSTANCE_LENGTH; k++){
			creature->cells[i].dv[k] = 0;
		}
	}
//    cout << "after clean / start of calc: \n";
//    printCreature_dv(creature);

    for(int i = 0; i < countOfCells; i++){ 
        for (int j = 0; j < genome->length; j++) {
            float * delta = (float*)malloc(genome->genes[j].cond_length * sizeof(float)); 
            for (int k = 0; k < genome->genes[j].cond_length; k++) {

//                cout << "cage " << i << " genome " << j << " cond " << k << " gen_sub " << (int)genome->genes[j].cond[k].substance <<
//                    " sub in v " << creature->cells[i].v[genome->genes[j].cond[k].substance]<<'\n';
                delta[k] = genome->genes[j].cond[k].sign 
                    ? creature->cells[i].v[genome->genes[j].cond[k].substance] - genome->genes[j].cond[k].threshold
                    : genome->genes[j].cond[k].threshold - creature->cells[i].v[genome->genes[j].cond[k].substance];
//                    cout << "cell#"<<i<<" sign = "<< (int)genome->genes[j].cond[k].sign  << " substance in " << (int)genome->genes[j].cond[k].substance << 
//                        " = " << (float)creature->cells[i].v[genome->genes[j].cond[k].substance] << " thereshold = " << 
//                        (int)genome->genes[j].cond[k].threshold << " delta = " << delta[k] << "\n";
            }
            for (int k = 0; k < genome->genes[j].oper_length; k++) {
                for (int p = 0; p < genome->genes[j].cond_length; p++) {
//                    cout << "cell#"<<i<<" sign = "<< (int)genome->genes[j].operons[k].sign << " substance in " << (int)genome->genes[j].operons[k].substance << 
//                            " = " << (float)creature->cells[i].dv[genome->genes[j].operons[k].substance] << " rate = " << 
//                            (int)genome->genes[j].operons[k].rate << " delta = " << delta[p] << " sigma(delta[p]) = "<< calc_sigma((float)(delta[p])/127) << '\n';
                    genome->genes[j].operons[k].sign 
                        ? creature->cells[i].dv[genome->genes[j].operons[k].substance] -= genome->genes[j].operons[k].rate * calc_sigma((float)(delta[p])/127)
                        : creature->cells[i].dv[genome->genes[j].operons[k].substance] += genome->genes[j].operons[k].rate * calc_sigma((float)(delta[p])/127);
//                        cout << " dv in " << (int)genome->genes[j].operons[k].substance << " = " 
//                            << (float)creature->cells[i].dv[genome->genes[j].operons[k].substance] << "\n\n\n\n";
                }
            }
            free(delta);
        }
    }
}

void init_blur_matrix(struct matrix ** matrix){
	*matrix = (struct matrix*)calloc(1, sizeof(struct matrix));
	(*matrix)->size = 3; 
	(*matrix)->val = (float*)calloc((*matrix)->size * (*matrix)->size, sizeof(float));
	(*matrix)->val[0] = 1;
	(*matrix)->val[1] = 2;
	(*matrix)->val[2] = 1;
	(*matrix)->val[3] = 2;
	(*matrix)->val[4] = 4;
	(*matrix)->val[5] = 2;
	(*matrix)->val[6] = 1;
	(*matrix)->val[7] = 2;
	(*matrix)->val[8] = 1;
	(*matrix)->norm_rate = 0;
	for(int i = 0; i < (*matrix)->size * (*matrix)->size; i++){
		(*matrix)->norm_rate += (*matrix)->val[i];
	}
}

void diff_v2(struct creature * creature, struct matrix * matrix) {
    int countOfCells = creature->n * creature->n;

    for (int i = 0; i < creature->n; i++) {
        for (int j = 0; j < creature->n; j++) {
            float accum[SUBSTANCE_LENGTH] = { 0 };
            int countOfCells = creature->n * creature->n;
            int sz = matrix->size / 2;  
            int k, l, p;
            int core_point = i * creature->n + j;
	        int cur_cell_i = i;
	        int cur_cell_j = j;
	        for (k = -sz; k <= sz; k++){
        		for (l = -sz; l <= sz; l++){
        			cur_cell_i = i + k;
        			cur_cell_j = j + l;
        			if(cur_cell_j < 0){//l < 0
        				cur_cell_j = -l;
        			}
        			if(cur_cell_j >= creature->n){//l > 0
        				cur_cell_j = creature->n - l - 1;
        			}
        			if(cur_cell_i < 0){ //здесь k < 0
        				cur_cell_i = -k;
        			}
        			if(cur_cell_i >= creature->n){//k > 0
        				cur_cell_i = creature->n - k - 1;
        			}
        			for(p = 0; p < SUBSTANCE_LENGTH; p++){
        				accum[p] += (creature->cells[cur_cell_i * creature->n + cur_cell_j].v[p] * matrix->val[(sz + k) * matrix->size + (sz + l)]);
        			}
                }
            }
            for (p = 0; p < SUBSTANCE_LENGTH; p++){
                creature->cells[core_point].dv[p] = (int)(accum[p])/matrix->norm_rate;
            }
    	}
    }
}



