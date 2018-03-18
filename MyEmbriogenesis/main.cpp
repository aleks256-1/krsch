#include <malloc.h>
#include "creature.h"
#include "main.h"
#include "rdev.h"
#include "genome.h"
#include "embriogenesis.h"
#include "bmpgen.h"

using namespace std;

int main() {
    struct creature *creature;
    struct genome *genome;
    struct matrix * matrix;
    genome = (struct genome*)malloc(sizeof(struct genome));

//   load(genome);
    initCreature(&creature);
    initRandGenome(genome);
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
        calculateDvForCells_v2(creature, genome);
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
    free(genome);
    free(creature);
}
