#include <malloc.h>
#include "creature.h"
#include "main.h"
#include "rdev.h"
#include "genome.h"
#include "embriogenesis.h"
#include "bmpgen.h"
#include <iostream>

#define START_GENERATION 10
#define IDEAL 10
#define NEXT_GENERATION 100

using namespace std;



void initImageNames(char ** imageNames) {
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
}

void crossParents(struct genome * first, struct genome * second) {

}

void copyGenome(struct genome * first, struct genome * second) {

}

int main() {
    struct creature *firstGeneration[START_GENERATION+1]; //#1000 - IdealCreature
    struct creature *nextGeneration[NEXT_GENERATION];
    struct matrix * matrix;
    struct genome *firstGenerationGenome[START_GENERATION + 1]; //#1000 - IdealCreature  
    struct genome *nextGenerationGenome[NEXT_GENERATION];
    
    init_blur_matrix(&matrix);

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////  INITIALIZING FIRST GENERATION ARRAYS  ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < START_GENERATION + 1; i++) {                                        ////// 
        initCreature(&firstGeneration[i]);                                                  //////
        cout << "creature # " << i << " initialized\n";                                     //////
    }                                                                                       //////
    for (int i = 1; i < START_GENERATION + 1; i++) {                                        //////
        for (int j = 0; j < firstGeneration[i]->n * firstGeneration[i]->n; j++) {           //////
            for (int k = 0; k < SUBSTANCE_LENGTH; k++) {                                    //////
                firstGeneration[i]->cells[j].v[k] = firstGeneration[0]->cells[j].v[k];      //////
            }                                                                               //////
        }                                                                                   ////// 
    }                                                                                       ////// 
    for (int i = 0; i < START_GENERATION + 1; i++) {                                        //////
        firstGenerationGenome[i] = (struct genome *)malloc(sizeof(struct genome));          //////
        initRandGenome(firstGenerationGenome[i]);                                           //////
        cout << "genome # " << i << " initialized\n";                                       //////
    }                                                                                       //////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  Growing up IdealCreature  /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    int step = 0;                                                                           //////
    int i = 0;                                                                              //////
    char* imageNames[FILENAME_MAX];                                                         //////
    initImageNames(imageNames);                                                             //////
    while(firstGeneration[IDEAL]->n < MAX_CREATURE_SIZE) {                                  //////
        calculateDvForCells_v2(firstGeneration[IDEAL], firstGenerationGenome[IDEAL]);       ////// 
        applyDvForCells(firstGeneration[IDEAL]);                                            //////
        diff_v2(firstGeneration[IDEAL], matrix);                                            //////
        applyDiff(firstGeneration[IDEAL]);                                                  //////
        if (step % GROW_SIZE == 0) {                                                        //////
            firstGeneration[IDEAL] = grow(firstGeneration[IDEAL]);                          //////
            createImage(firstGeneration[IDEAL], (char*)imageNames[i]);                      //////
            i++;                                                                            //////
        }                                                                                   //////
        step++;                                                                             //////
        cout << step << '\n';                                                               //////
    }                                                                                       //////
    createImage(firstGeneration[IDEAL], (char*)"IDEAL.bmp");                                //////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////  INITIALIZING NEXT GENERATION ARRAYS  ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < NEXT_GENERATION; i++) {                                             ////// 
        initCreature(&nextGeneration[i]);                                                   //////
        cout << "creature # " << i << " initialized\n";                                     //////
    }                                                                                       //////
    for (int i = 0; i < NEXT_GENERATION; i++) {                                             //////
        for (int j = 0; j < nextGeneration[i]->n * nextGeneration[i]->n; j++) {             //////
            for (int k = 0; k < SUBSTANCE_LENGTH; k++) {                                    //////
                nextGeneration[i]->cells[j].v[k] = firstGeneration[0]->cells[j].v[k];       //////
            }                                                                               //////
        }                                                                                   ////// 
    }                                                                                       //////
    for (int i = 0; i < START_GENERATION; i++) {                                            //////
        nextGenerationGenome[i] = (struct genome *)malloc(sizeof(struct genome));           //////
        copyGenome(firstGenerationGenome[i], nextGenerationGenome[i]);                      //////
        cout << "genome # " << i << " initialized\n";                                       //////
    }                                                                                       //////
    for (int i = START_GENERATION; i < NEXT_GENERATION; i++) {                              //////
        nextGenerationGenome[i] = (struct genome *)malloc(sizeof(struct genome));           //////
        cout << "genome # " << i << " initialized\n";                                       //////
    }                                                                                       //////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    
///////////////////////////////////  CROSSING RANDOM GENOMES /////////////////////////////////////
    for (int i = START_GENERATION; i < NEXT_GENERATION; i++) {    
        crossParents(firstGenerationGenome[RangedRandomNumber(0,999)], firstGenerationGenome[RangedRandomNumber(0,999)]); 
    }








    for (int i = 0; i < NEXT_GENERATION; i++) {                                             
        free(nextGeneration[i]);                                          
    }   
    for (int i = 0; i < START_GENERATION + 1; i++) {
        free(firstGenerationGenome[i]);
        free(firstGeneration[i]);
    }
}
