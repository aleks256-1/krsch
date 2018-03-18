#include "genome.h"
#include "rdev.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <bitset>
#include <iostream>

using namespace std;

void initRandGenome(struct genome * genome){
	genome->length = RangedRandomNumber(1, MAX_GENOME_SIZE);
    cout << "genome_length = " << genome->length << '\n';
	genome->genes = (struct gene*)calloc(genome->length, sizeof(struct gene));
	for(int i = 0; i < genome->length; i++){
		genome->genes[i].cond_length = RangedRandomNumber(1, MAX_COND_LENGTH);
		genome->genes[i].cond = (struct cond*)calloc(genome->genes[i].cond_length, sizeof(struct cond));
		for(int j = 0; j < genome->genes[i].cond_length; j++){
            genome->genes[i].cond[j].threshold = RangedRandomNumber(0, MAX_COND_VALUE);
            genome->genes[i].cond[j].substance = RangedRandomNumber(0, MAX_COND_VALUE);
            genome->genes[i].cond[j].sign = RangedRandomNumber(0,1);
            cout << "[" << (int)genome->genes[i].cond[j].substance << "] ";
            if (genome->genes[i].cond[j].sign == 1) {
                cout << '>';
            } else {
                cout << '<';
            }
            cout << " " << (int)genome->genes[i].cond[j].threshold;
            if (j != genome->genes[i].cond_length - 1) {
                cout << ", ";
            }
        }
        cout << " = ";
		genome->genes[i].oper_length = RangedRandomNumber(1, MAX_OPERON_LENGTH);
		genome->genes[i].operons = (struct operon*)calloc(genome->genes[i].oper_length, sizeof(struct operon));
		for(int j = 0; j < genome->genes[i].oper_length; j++){
            genome->genes[i].operons[j].rate = RangedRandomNumber(0, MAX_OPERON_VALUE);
            srand(j * 2 * genome->length + 1);
            genome->genes[i].operons[j].substance = RangedRandomNumber(0, MAX_OPERON_VALUE);
            srand(j * 2 * genome->length + 2);
            genome->genes[i].operons[j].sign = RangedRandomNumber(0, 1);
            cout << "[" << (int)genome->genes[i].operons[j].substance << "] ";
            if (genome->genes[i].operons[j].sign == 1) {
                cout << '-';
            } else {
                cout << '+';
            }
            cout << " " << (int)genome->genes[i].operons[j].rate;
            if (j != genome->genes[i].oper_length - 1) {
                cout << ", ";
            }
		}
        cout << "\n\n";
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

void loadGenome(struct genome * genome) {
    FILE * fp;
    fp = fopen("genome.txt", "r");
    int i;
    char line[256];

    genome->length = 0;
    while(fgets(line, sizeof(line), fp)) {
        if (line[0] == '[') {
            genome->length++;
        }
    }
    genome->genes = (struct gene*)calloc(genome->length, sizeof(struct gene));
    for (i = 0; i < genome->length; i++) {
        genome->genes[i].cond_length = 1;
        genome->genes[i].oper_length = 1;
        genome->genes[i].cond = (struct cond*)calloc(genome->genes[i].cond_length, sizeof(struct cond));
		genome->genes[i].operons = (struct operon*)calloc(genome->genes[i].oper_length, sizeof(struct operon));
    }

    int current_gene = 0;
    int current_cond_oper = 0;
    rewind(fp);
    while(fgets(line, sizeof(line), fp)) {
        if (line[0] != '[') {
            continue;
        }
        i = 1;
        int temp = 0;
        while (line[i] != ']') {
            temp *= 10;
            temp += line[i] - '0';
            i++;
        }
        genome->genes[current_gene].cond[current_cond_oper].substance = temp;
        i+=2;
        if (line[i] == '>') {
            genome->genes[current_gene].cond[current_cond_oper].sign = 1;
        } else {
            genome->genes[current_gene].cond[current_cond_oper].sign = 0;
        }
        i+=2;
        temp = 0;
        while (line[i] != ' ') {
            temp *= 10;
            temp += line[i] - '0';
            i++;
        }
        genome->genes[current_gene].cond[current_cond_oper].threshold = temp;
        i+=4;
        temp = 0;
        while (line[i] != ']') {
            temp *= 10;
            temp += line[i] - '0';
            i++;
        }
        genome->genes[current_gene].operons[current_cond_oper].substance = temp;
        i+=2;
        if (line[i] == '-') {
            genome->genes[current_gene].operons[current_cond_oper].sign = 1;
        } else {
            genome->genes[current_gene].operons[current_cond_oper].sign = 0;
        }
        i+=2;
        temp = 0;
        while (line[i] != '\n') {
            temp *= 10;
            temp += line[i] - '0';
            i++;
        }
        genome->genes[current_gene].operons[current_cond_oper].rate = temp;
        current_gene++;
    }

    fclose(fp);
}