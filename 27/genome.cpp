#include "genome.h"
#include "rdev.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <bitset>
#include <iostream>
#include <time.h>


using namespace std;

void copyGenome_uint16(uint16_genome * genome1, uint16_genome * genome2) {
	genome2->size = genome1->size;
    genome2->genes = (uint16_t*)malloc(genome2->size * sizeof(uint16_t));
    for (int i = 0; i < genome2->size; i++) {
        genome2->genes[i] = genome1->genes[i];
    }
}


void Print_uint16_genome(struct uint16_genome * genome) {
    for (int i = 0; i < genome->size; i++) {
        cout << genome->genes[i] << ' ';
    }
    cout << '\n';
}

void initRandGenome_uint16(uint16_genome * genome) {
    genome->size = RangedRandomNumber(MIN_GENOME_SIZE, MAX_GENOME_SIZE);
    genome->genes = (uint16_t*)malloc(genome->size * sizeof(uint16_t));
    for (int i = 0; i < genome->size; i++) {
        genome->genes[i] = RangedRandomNumber(0, MAX_UINT16);
    }
}



void printGenome(struct genome * genome) {
    cout << "genome_length = " << genome->length << '\n';
	for(int i = 0; i < genome->length; i++){
		for(int j = 0; j < genome->genes[i].cond_length; j++){
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
		for(int j = 0; j < genome->genes[i].oper_length; j++){
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

void copyGenome(struct genome * genome1, struct genome * genome2) {
	saveGenome(genome1);
	loadGenome(genome2);
}

void initRandGenome(struct genome * genome){
	genome->length = RangedRandomNumber(1, MAX_GENOME_SIZE);
    genome->genes = (struct gene*)calloc(genome->length, sizeof(struct gene));
	for(int i = 0; i < genome->length; i++) {
		genome->genes[i].cond_length = RangedRandomNumber(1, MAX_COND_LENGTH);
		genome->genes[i].cond = (struct cond*)calloc(genome->genes[i].cond_length, sizeof(struct cond));
		for(int j = 0; j < genome->genes[i].cond_length; j++) {
            genome->genes[i].cond[j].threshold = RangedRandomNumber(0, MAX_COND_VALUE);
            genome->genes[i].cond[j].substance = RangedRandomNumber(0, MAX_COND_VALUE);
            genome->genes[i].cond[j].sign = RangedRandomNumber(0,1);
        }
        genome->genes[i].oper_length = RangedRandomNumber(1, MAX_OPERON_LENGTH);
		genome->genes[i].operons = (struct operon*)calloc(genome->genes[i].oper_length, sizeof(struct operon));
		for(int j = 0; j < genome->genes[i].oper_length; j++){
            genome->genes[i].operons[j].rate = RangedRandomNumber(0, MAX_OPERON_VALUE);
            srand(j * 2 * genome->length + 1);
            genome->genes[i].operons[j].substance = RangedRandomNumber(0, MAX_OPERON_VALUE);
            srand(j * 2 * genome->length + 2);
            genome->genes[i].operons[j].sign = RangedRandomNumber(0, 1);
        }
	}
}

void saveGenome(struct genome * genome){
    FILE *fp;
    const char * path = (const char *)"genome.txt";
	if ((fp = fopen(path, "wb"))==NULL) {
		printf ("Cannot open genome file.\n");
		return;
    }
	int i, j;
	for(i = 0; i < genome->length; i++){
		for(j = 0; j < genome->genes[i].cond_length; j++){
			uint16_t cond = 0;
			cond |= ((uint16_t)genome->genes[i].cond[j].sign) << 14;
			cond |= (uint16_t)genome->genes[i].cond[j].substance;
            cond |= ((uint16_t)genome->genes[i].cond[j].threshold) << 7;
            cond &= 0x7fff;
			bitset<16> x(cond);
			fwrite(&cond, sizeof(uint16_t), 1, fp);
		}
		for(j = 0; j < genome->genes[i].oper_length; j++){
			uint16_t oper = 0;
			oper |=	1 << 15;
			oper |= ((uint16_t)genome->genes[i].operons[j].sign) << 14;
			oper |= (uint16_t)genome->genes[i].operons[j].substance;
			oper |= ((uint16_t)genome->genes[i].operons[j].rate) << 6;
            bitset<16> x(oper);
            fwrite(&oper, sizeof(uint16_t), 1, fp);
		}
	}
	fclose(fp);
}
bool get_flag(uint16_t val){
	return (val & 0x8000) >> 15;
}

unsigned char get_substance(uint16_t val){
	return val & 0x7f;
}

unsigned char get_sign(uint16_t val){
	return (val & 0x4000) >> 14;
}

unsigned char get_threshold(uint16_t val){
	return (val & 0x3f80) >> 7;
}

unsigned char get_rate(uint16_t val){
	return (val & 0x3f80) >> 7;
}

void genome_to_uint16genome(struct uint16_genome * genome1, struct genome * genome2) {
    int i, j;
    int size = 0;
    for (int i = 0; i < genome2->length; i++) {
        size += genome2->genes[i].cond_length + genome2->genes[i].oper_length;
    }
    cout << "size = " << size << ' ';
    genome1->size = size;
    genome1->genes = (uint16_t *)malloc(genome1->size * sizeof(uint16_t));
    int k = 0;
	for(i = 0; i < genome2->length; i++) {
		for(j = 0; j < genome2->genes[i].cond_length; j++){
			genome1->genes[k] = 0;
			genome1->genes[k] |= ((uint16_t)genome2->genes[i].cond[j].sign) << 14;
			genome1->genes[k] |= (uint16_t)genome2->genes[i].cond[j].substance;
            genome1->genes[k] |= ((uint16_t)genome2->genes[i].cond[j].threshold) << 7;
            genome1->genes[k] &= 0x7fff;
            bitset<16> x(genome1->genes[j]);
            k++;
		}
		for(j = 0; j < genome2->genes[i].oper_length; j++){
			genome1->genes[k] = 0;
			genome1->genes[k] |= 0x8000;
			genome1->genes[k] |= ((uint16_t)genome2->genes[i].operons[j].sign) << 14;
			genome1->genes[k] |= (uint16_t)genome2->genes[i].operons[j].substance;
			genome1->genes[k] |= ((uint16_t)genome2->genes[i].operons[j].rate) << 7;
            bitset<16> x(genome1->genes[k]);
            k++;
        }
	}
}

void uint16genome_to_genome(struct uint16_genome * genome1, struct genome * genome2) {
    bool flag = false;
	genome2->length = 0;
	int i = 0;
	while (i < genome1->size && get_flag(genome1->genes[i])) {
			i++;
	}
	int startPos = i;
	while (i < genome1->size) {
		while (i < genome1->size && !get_flag(genome1->genes[i])) {
            i++;
            flag = true;
		}
		while (i < genome1->size && get_flag(genome1->genes[i])) {
            i++;
            flag = false;
        }
        if (flag == true) {
            continue;
        }
		genome2->length += 1;
    }
	genome2->genes = (struct gene*)calloc(genome2->length, sizeof(struct gene));
	int pos = 0;
	i = startPos;
	for (pos = 0; pos < genome2->length; pos++) {
		genome2->genes[pos].oper_length = 0;
		genome2->genes[pos].cond_length = 0;
		while (i < genome1->size && !get_flag(genome1->genes[i])) {
			i++;
			genome2->genes[pos].cond_length += 1;
		}
		while (i < genome1->size && get_flag(genome1->genes[i])) {
			i++;
			genome2->genes[pos].oper_length += 1;
		}
		
	}
	for(i = 0; i < genome2->length; i++){
		genome2->genes[i].cond = (struct cond*)calloc(genome2->genes[i].cond_length, sizeof(struct cond));
		genome2->genes[i].operons = (struct operon*)calloc(genome2->genes[i].oper_length, sizeof(struct operon));
	}

	pos = 0;
	int cur_cond = 0;
	int cur_oper = 0;
	i = startPos;
	while (pos < genome2->length){
		while(!get_flag(genome1->genes[i])) {
			genome2->genes[pos].cond[cur_cond].substance = get_substance(genome1->genes[i]);
			genome2->genes[pos].cond[cur_cond].sign = get_sign(genome1->genes[i]);
			genome2->genes[pos].cond[cur_cond].threshold = get_threshold(genome1->genes[i]);
			cur_cond++;
			i++;
		}
		while(get_flag(genome1->genes[i])) {
			genome2->genes[pos].operons[cur_oper].substance = get_substance(genome1->genes[i]);
			genome2->genes[pos].operons[cur_oper].sign = get_sign(genome1->genes[i]);
			genome2->genes[pos].operons[cur_oper].rate = get_rate(genome1->genes[i]);
			cur_oper++;
			i++;
		}
		pos++;
		cur_cond = 0;
		cur_oper = 0;
	}
}


void loadGenome(struct genome * genome){
    FILE *fp;
    const char * path = (const char *)"genome.txt";
	if ((fp = fopen(path, "rb"))==NULL) {
		printf ("Cannot open genome file.\n");
		return;
	}
	fseek(fp,0,SEEK_END);
    int size = (ftell(fp))/2;
	rewind(fp);
	uint16_t* buffer = (uint16_t*)malloc(size * sizeof(uint16_t));
	if(fread(buffer, sizeof(uint16_t), size, fp) != size){
		printf("Error on reading genome!\n");
		return;
	}
	bool oper_flag = false;
	genome->length = 0;
	int i = 0;
	while (i < size && get_flag(buffer[i])) {
			i++;
	}
	int startPos = i;
	while (i < size) {
		while (i < size && !get_flag(buffer[i])) {
			i++;
		}
		while (i < size && get_flag(buffer[i])) {
			i++;
		}
		genome->length += 1;
	}
	genome->length -= 1;
	genome->genes = (struct gene*)calloc(genome->length, sizeof(struct gene));
	
	int pos = 0;
	i = startPos;
	for (pos = 0; pos < genome->length; pos++) {
		genome->genes[pos].oper_length = 0;
		genome->genes[pos].cond_length = 0;
		while (i < size && !get_flag(buffer[i])) {
			i++;
			genome->genes[pos].cond_length += 1;
		}
		while (i < size && get_flag(buffer[i])) {
			i++;
			genome->genes[pos].oper_length += 1;
		}
		
	}
	for(i = 0; i < genome->length; i++){
		genome->genes[i].cond = (struct cond*)calloc(genome->genes[i].cond_length, sizeof(struct cond));
		genome->genes[i].operons = (struct operon*)calloc(genome->genes[i].oper_length, sizeof(struct operon));
	}

	pos = 0;
	int cur_cond = 0;
	int cur_oper = 0;
	i = startPos;
	while (pos < genome->length && i < size){
		while(!get_flag(buffer[i])) {
			genome->genes[pos].cond[cur_cond].substance = get_substance(buffer[i]);
			genome->genes[pos].cond[cur_cond].sign = get_sign(buffer[i]);
			genome->genes[pos].cond[cur_cond].threshold = get_threshold(buffer[i]);
			cur_cond++;
			i++;
		}
		while(get_flag(buffer[i])) {
			genome->genes[pos].operons[cur_oper].substance = get_substance(buffer[i]);
			genome->genes[pos].operons[cur_oper].sign = get_sign(buffer[i]);
			genome->genes[pos].operons[cur_oper].rate = get_rate(buffer[i]);
			cur_oper++;
			i++;
		}
		pos++;
		cur_cond = 0;
		cur_oper = 0;

	}

/*	for(i = 0; i < genome->length; i++){
		for(int j = 0; j < genome->genes[i].cond_length; j++){
			printf("cond = %d %d %d\n", genome->genes[i].cond[j].substance, genome->genes[i].cond[j].sign, genome->genes[i].cond[j].threshold);
		}
		for(int j = 0; j < genome->genes[i].oper_length; j++){
			printf("oper = %d %d %d\n", genome->genes[i].operons[j].substance, genome->genes[i].operons[j].sign, genome->genes[i].operons[j].rate);
		}
	}
*/	free(buffer);
	fclose(fp);
}

void loadGenome_v1(struct genome * genome) {
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
