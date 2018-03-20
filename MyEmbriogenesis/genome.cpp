#include "genome.h"
#include "rdev.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <bitset>
#include <iostream>
#include <time.h>


using namespace std;

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


void initRandGenome(struct genome * genome){
	genome->length = 1;
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
	return (val & 0x3f00) >> 7;
}

unsigned char get_rate(uint16_t val){
	return (val & 0x3f00) >> 6;
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
	while (i < size) {
		while (i < size && get_flag(buffer[i])) {
			i++;
		}
		while (i < size && !get_flag(buffer[i])) {
			i++;
		}
		genome->length += 1;
	}
	genome->length -= 1;
	genome->genes = (struct gene*)calloc(genome->length, sizeof(struct gene));
	
	int pos = 0;
	i = 0;
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
	oper_flag = false;
	int cur_cond = 0;
	int cur_oper = 0;
	for(i = 0; i < size; i++){
		if(oper_flag == false){
			oper_flag = get_flag(buffer[i]);
			if(oper_flag == true){
				genome->genes[pos].operons[cur_oper].substance = get_substance(buffer[i]);
				genome->genes[pos].operons[cur_oper].sign = get_sign(buffer[i]);
				genome->genes[pos].operons[cur_oper].rate = get_rate(buffer[i]);
				cur_oper++;
			}
			else{
				genome->genes[pos].cond[cur_cond].substance = get_substance(buffer[i]);
				genome->genes[pos].cond[cur_cond].sign = get_sign(buffer[i]);
				genome->genes[pos].cond[cur_cond].threshold = get_threshold(buffer[i]);
				cur_cond++;
			}
		}
		else{
			oper_flag = get_flag(buffer[i]);
			if(oper_flag == true){
				genome->genes[pos].operons[cur_oper].substance = get_substance(buffer[i]);
				genome->genes[pos].operons[cur_oper].sign = get_sign(buffer[i]);
				genome->genes[pos].operons[cur_oper].rate = get_rate(buffer[i]);
				cur_oper++;
			}
			else{
				pos++;
				cur_cond = cur_oper = 0;
				genome->genes[pos].cond[cur_cond].substance = get_substance(buffer[i]);
				genome->genes[pos].cond[cur_cond].sign = get_sign(buffer[i]);
				genome->genes[pos].cond[cur_cond].threshold = get_threshold(buffer[i]);
				cur_cond++;
			}
		}
	}
	for(i = 0; i < genome->length; i++){
		for(int j = 0; j < genome->genes[i].cond_length; j++){
			printf("cond = %d %d %d\n", genome->genes[i].cond[j].substance, genome->genes[i].cond[j].sign, genome->genes[i].cond[j].threshold);
		}
		for(int j = 0; j < genome->genes[i].oper_length; j++){
			printf("oper = %d %d %d\n", genome->genes[i].operons[j].substance, genome->genes[i].operons[j].sign, genome->genes[i].operons[j].rate);
		}
	}
	free(buffer);
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
