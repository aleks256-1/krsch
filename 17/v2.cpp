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
#include <string> 
#include <fstream>

#define SUBSTANCE_LENGTH 128
#define MAX_CREATURE_SIZE 512
#define N 4
#define MAX_GENOME_SIZE 30
#define MAX_COND_LENGTH 32
#define MAX_OPERON_LENGTH 32
#define MAX_COND_VALUE 127
#define MAX_OPERON_VALUE 127
#define fileHeaderSize 14
#define infoHeaderSize 40
#define bytesPerPixel 3

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

struct matrix{
	int size;
	float *val;
	float norm_rate;
};


float calc_sigma(float x){
	return (1 / (1 + exp(-x)));
}

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
	(*creature)->cells[0].v[0] = (*creature)->cells[0].dv[0] = 255;
	(*creature)->cells[1].v[0] = (*creature)->cells[1].dv[0] = 255;
	(*creature)->cells[2].v[0] = (*creature)->cells[2].dv[0] = 255;
	(*creature)->cells[3].v[0] = (*creature)->cells[3].dv[0] = 255;
	(*creature)->cells[4].v[0] = (*creature)->cells[4].dv[0] = 255;
	(*creature)->cells[5].v[0] = (*creature)->cells[5].dv[0] = 255;
	(*creature)->cells[6].v[0] = (*creature)->cells[6].dv[0] = 255;
	(*creature)->cells[7].v[0] = (*creature)->cells[7].dv[0] = 255;
	(*creature)->cells[3].v[1] = (*creature)->cells[3].dv[1] = 255; 
	(*creature)->cells[7].v[1] = (*creature)->cells[7].dv[1] = 255;
	(*creature)->cells[11].v[1] = (*creature)->cells[11].dv[1] = 255;
	(*creature)->cells[15].v[1] = (*creature)->cells[15].dv[1] = 255;
	(*creature)->cells[2].v[1] = (*creature)->cells[2].dv[1] = 255; 
	(*creature)->cells[6].v[1] = (*creature)->cells[6].dv[1] = 255;
	(*creature)->cells[10].v[1] = (*creature)->cells[10].dv[1] = 255;
	(*creature)->cells[14].v[1] = (*creature)->cells[14].dv[1] = 255;
    
    
}

void load(struct genome * genome) {
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

void calculateDvForCells_v2(struct creature * creature, struct genome * genome) {
    int countOfCells = creature->n * creature->n;
    for(int i = 0; i < countOfCells; i++){ 
        for (int j = 0; j < genome->length; j++) {
            int * delta = (int*)malloc(genome->genes[j].cond_length * sizeof(int)); 
            for (int k = 0; k < genome->genes[j].cond_length; k++) {
                delta[k] = genome->genes[j].cond[k].sign 
                    ? creature->cells[i].v[genome->genes[j].cond[k].substance] - genome->genes[j].cond[k].threshold
                    : genome->genes[j].cond[k].threshold - creature->cells[i].v[genome->genes[j].cond[k].substance];
                    cout << "cell#"<<i<<" sign = "<< (int)genome->genes[j].cond[k].sign  << " substance in " << (int)genome->genes[j].cond[k].substance << 
                        " = " << (float)creature->cells[i].v[genome->genes[j].cond[k].substance] << " thereshold = " << 
                        (int)genome->genes[j].cond[k].threshold << " delta = " << delta[k] << '\n';}
            for (int k = 0; k < genome->genes[j].oper_length; k++) {
                for (int p = 0; p < genome->genes[j].cond_length; p++) {
                    cout << "cell#"<<i<<" sign = "<< (int)genome->genes[j].operons[k].sign << " substance in " << (int)genome->genes[j].operons[k].substance << 
                            " = " << (float)creature->cells[i].dv[genome->genes[j].operons[k].substance] << " rate = " << 
                            (int)genome->genes[j].operons[k].rate << " delta = " << delta[p] << " sigma(delta[p]) = "<< calc_sigma(delta[p]) << '\n';
                    genome->genes[j].operons[k].sign 
                        ? creature->cells[i].dv[genome->genes[j].operons[k].substance] -= genome->genes[j].operons[k].rate * calc_sigma(delta[p]/127)
                        : creature->cells[i].dv[genome->genes[j].operons[k].substance] += genome->genes[j].operons[k].rate * calc_sigma(delta[p]/127);
                        cout << " dv in " << (int)genome->genes[j].operons[k].substance << " = " 
                            << (float)creature->cells[i].dv[genome->genes[j].operons[k].substance] << '\n';
                }
            }
            free(delta);
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

unsigned char* createBitmapFileHeader(int height, int width){
    int fileSize = fileHeaderSize + infoHeaderSize + bytesPerPixel*height*width;

    static unsigned char fileHeader[] = {
        0,0, /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };
    fileHeader[ 0] = (unsigned char)('B');
    fileHeader[ 1] = (unsigned char)('M');
    fileHeader[ 2] = (unsigned char)(fileSize    );
    fileHeader[ 3] = (unsigned char)(fileSize>> 8);
    fileHeader[ 4] = (unsigned char)(fileSize>>16);
    fileHeader[ 5] = (unsigned char)(fileSize>>24);
    fileHeader[10] = (unsigned char)(fileHeaderSize + infoHeaderSize);

    return fileHeader;
}

unsigned char* createBitmapInfoHeader(int height, int width){
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0, /// number of color planes
        0,0, /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };
    infoHeader[ 0] = (unsigned char)(infoHeaderSize);
    infoHeader[ 4] = (unsigned char)(width    );
    infoHeader[ 5] = (unsigned char)(width>> 8);
    infoHeader[ 6] = (unsigned char)(width>>16);
    infoHeader[ 7] = (unsigned char)(width>>24);
    infoHeader[ 8] = (unsigned char)(height    );
    infoHeader[ 9] = (unsigned char)(height>> 8);
    infoHeader[10] = (unsigned char)(height>>16);
    infoHeader[11] = (unsigned char)(height>>24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(bytesPerPixel*8);

    return infoHeader;
}

void generateBitmapImage(unsigned char *image, int height, int width, char* imageFileName){
    unsigned char* fileHeader = createBitmapFileHeader(height, width);
    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4-(width*bytesPerPixel)%4)%4;
    FILE* imageFile = fopen(imageFileName, "wb");
    fwrite(fileHeader, 1, fileHeaderSize, imageFile);
    fwrite(infoHeader, 1, infoHeaderSize, imageFile);
    int i;
    for(i=0; i<height; i++){
        fwrite(image+(i*width*bytesPerPixel), bytesPerPixel, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }
    fclose(imageFile);
}

void createImage(struct creature * creature, char* imageFileName) {
    unsigned char image[creature->n][creature->n][bytesPerPixel];
    for(int i = 0; i < creature->n; i++){
        for(int j = 0; j < creature->n; j++){
            image[i][j][2] = (unsigned char)(creature->cells[i * creature->n + j].v[2]); ///red
            image[i][j][1] = (unsigned char)(creature->cells[i * creature->n + j].v[3]); ///green
            image[i][j][0] = (unsigned char)(creature->cells[i * creature->n + j].v[4]); ///blue
        }
    }
    generateBitmapImage((unsigned char *)image, creature->n, creature->n, imageFileName);
}


/**************************************************************************************************************/
/**************************************************************************************************************/
/**************************************************************************************************************/

int main() {
    struct creature *creature;
    struct genome *genome;
    struct matrix * matrix;
    genome = (struct genome*)malloc(sizeof(struct genome));

    load(genome);
    initCreature(&creature);
//    initRandGenome(genome);
    init_blur_matrix(&matrix);
 //   saveGenome(genome);
    char* imageNames[FILENAME_MAX];
    imageNames[0] = (char*)"1.bmp";
    imageNames[1] = (char*)"2.bmp";
    imageNames[2] = (char*)"3.bmp";
    imageNames[3] = (char*)"4.bmp";
    imageNames[4] = (char*)"5.bmp";
    imageNames[5] = (char*)"6.bmp";
    imageNames[6] = (char*)"7.bmp";
    imageNames[7] = (char*)"8.bmp";
    imageNames[8] = (char*)"9.bmp";
    imageNames[9] = (char*)"10.bmp";
    imageNames[10] = (char*)"11.bmp";
    imageNames[11] = (char*)"12.bmp";
    imageNames[12] = (char*)"13.bmp";
    imageNames[13] = (char*)"14.bmp";
    imageNames[14] = (char*)"15.bmp";
    imageNames[15] = (char*)"16.bmp";
    imageNames[16] = (char*)"17.bmp";
    imageNames[17] = (char*)"18.bmp";
    int step = 0;
    int i = 0;
    while(creature->n < MAX_CREATURE_SIZE) {
        cout<<"############################ "<< step << " ############################\ncurrent creature\n\n";
        printCreature(creature);
        calculateDvForCells_v2(creature, genome);
        cout << "-----------------------------------------------------\n";
        printCreature_dv(creature);
        cout << "-----------------------------------------------------\n";
        applyDvForCells(creature); 
        diff_v2(creature, matrix);
        applyDiff(creature);
        if (step % 2 == 0) {
            createImage(creature, (char*)imageNames[i]);
            i++;
            creature = grow(creature);
        }
        step++;
    }
    createImage(creature, (char*)imageNames[i]);
    printCreature(creature);
    free(genome);
    free(creature);
}
