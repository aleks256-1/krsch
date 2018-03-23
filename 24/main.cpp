#include <malloc.h>
#include "creature.h"
#include "main.h"
#include "rdev.h"
#include "genome.h"
#include "embriogenesis.h"
#include "bmpgen.h"
#include <iostream>
#include "cross.h"
#include "mutations.h"
#include <bitset>

using namespace std;

int main() {
    struct uint16_genome * firstGeneration[START_POPULATION], * nextGeneration[NEXT_GENERATION_POPULATION];
    struct genome * genome = (struct genome *)malloc(sizeof(struct genome));
//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////  INITIALIZING FIRST GENERATION ARRAYS  ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < START_POPULATION; i++) {
        firstGeneration[i] = (struct uint16_genome *)malloc(sizeof(struct uint16_genome));
        initRandGenome_uint16(firstGeneration[i]);
    }                                                  
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////  INITIALIZING NEXT GENERATION ARRAYS  ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < START_POPULATION; i++) {
        nextGeneration[i] = (struct uint16_genome *)malloc(sizeof(struct uint16_genome));
        copyGenome_uint16(firstGeneration[i], nextGeneration[i]);
    }
    for (int i = START_POPULATION; i < NEXT_GENERATION_POPULATION; i++) {
        nextGeneration[i] = (struct uint16_genome *)malloc(sizeof(struct uint16_genome)); 
        crossGenome_uint16(firstGeneration[RangedRandomNumber(0, START_POPULATION - 1)], 
                            firstGeneration[RangedRandomNumber(0, START_POPULATION - 1)],
                            nextGeneration[i]);
    }
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  MUTATIONS  ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    int numberOfMutations = RangedRandomNumber(NEXT_GENERATION_POPULATION/2, NEXT_GENERATION_POPULATION); 
    for (int i = 0; i < numberOfMutations; i++) {                                           
        int mutation = RangedRandomNumber(0,100);                                          
        int creature = RangedRandomNumber(0, NEXT_GENERATION_POPULATION - 1);                        
        if (mutation < 13) {                     
            ChangeRandomBits(nextGeneration[creature]);                           
        } else if (mutation < 25) {    
            RandomFragmentDeletion(nextGeneration[creature]);                          
        } else if (mutation < 38) {                                                         
            RandomFragmentInsetrion(nextGeneration[creature]);                         
        } else if (mutation < 50) {    
            RandomFragmentDuplicate(nextGeneration[creature]);                    
        } else if (mutation < 63) {                                    
            RandomFragmentMove(nextGeneration[creature]);                            
        } else if (mutation < 75) {  
            RandomFragmentCopy(nextGeneration[creature]);                            
        } else if (mutation < 88) {                                                       
            int secondCreature = RangedRandomNumber(0, NEXT_GENERATION_POPULATION - 1);
            ExchangeDnaFragments(nextGeneration[creature], nextGeneration[secondCreature]);
        } else if (mutation < 100) { 
            RandomFragmentReverse(nextGeneration[creature]);
        }                                                                                   
    }                                                                                      
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  FREE MEMORY  ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
        free(nextGeneration[i]);
    }
    for (int i = 0; i < START_POPULATION; i++) {
        free(firstGeneration[i]);
    }
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    return 0;
}
