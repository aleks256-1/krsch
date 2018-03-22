#include <iostream>
#include <vector>
#include "rdev.h"
#include "genome.h"

#define CHANGE_BITS_PROPABILITY 25
#define REVERSE_BITS_PROPABILITY 25
#define MAX_FRAGMENTS_TO_INSERT 10

void RandomFragmentReverse(struct uint16_genome * genome) {
    for (int i = 0; i < genome->size; i++) {
        if (RangedRandomNumber(0,100) < REVERSE_BITS_PROPABILITY) {
            uint16_t x = 0, n = genome->genes[i];
            for(int i = 15; n; ) {
                x |= (n & 1) << i;
                n >>= 1;
                -- i;
            }
            genome->genes[i] = x;
        }
    }
}

void ExchangeDnaFragments(struct uint16_genome * genome1, struct uint16_genome * genome2) {
    int pos1 = RangedRandomNumber(0, genome1->size - 1);
    int pos2 = RangedRandomNumber(0, genome2->size - 1);
    int fragmentsToExchange1 = RangedRandomNumber(1, genome1->size - pos1);
    int fragmentsToExchange2 = RangedRandomNumber(1, genome2->size - pos2);
    uint16_t * fragment1 = (uint16_t*)malloc(fragmentsToExchange1 * sizeof(uint16_t));
    for (int i = pos1, j = 0; j < fragmentsToExchange1; i++,j++) {
        fragment1[j] = genome1->genes[i];
    }
    uint16_t * fragment2 = (uint16_t*)malloc(fragmentsToExchange2 * sizeof(uint16_t));
    for (int i = pos2, j = 0; j < fragmentsToExchange2; i++,j++) {
        fragment2[j] = genome2->genes[i];
    }
    int newSize1 = genome1->size - fragmentsToExchange1 + fragmentsToExchange2;
    uint16_t* buffer1 = (uint16_t*)malloc(newSize1 * sizeof(uint16_t));
    for (int i = 0; i < pos1; i++) {
        buffer1[i] = genome1->genes[i];
    }
    for (int i = pos1, j = 0; j < fragmentsToExchange2; i++, j++) {
        buffer1[i] = fragment2[j];
    }
    for (int i = pos1 + fragmentsToExchange2, j = pos1 + fragmentsToExchange1; i < newSize1; i++, j++) {
        buffer1[i] = genome1->genes[j];
    }
    int newSize2 = genome2->size - fragmentsToExchange2 + fragmentsToExchange1;
    uint16_t* buffer2 = (uint16_t*)malloc(newSize2 * sizeof(uint16_t));
    for (int i = 0; i < pos2; i++) {
        buffer2[i] = genome2->genes[i];
    }
    for (int i = pos2, j = 0; j < fragmentsToExchange1; i++, j++) {
        buffer2[i] = fragment1[j];
    }
    for (int i = pos2 + fragmentsToExchange1, j = pos2 + fragmentsToExchange2; i < newSize2; i++, j++) {
        buffer2[i] = genome2->genes[j];
    }
    genome1->size = newSize1;
    free(genome1->genes);
    genome1->genes = (uint16_t*)malloc(genome1->size * sizeof(uint16_t));
    for (int i = 0; i < genome1->size; i++) {
        genome1->genes[i] = buffer1[i];
    }  
    genome2->size = newSize2;
    free(genome2->genes);
    genome2->genes = (uint16_t*)malloc(genome2->size * sizeof(uint16_t));
    for (int i = 0; i < genome2->size; i++) {
        genome2->genes[i] = buffer2[i];
    } 
    free(fragment1);
    free(fragment2);
    free(buffer1);
    free(buffer2);

}

void RandomFragmentCopy(struct uint16_genome * genome) {
    int pos = RangedRandomNumber(0, genome->size - 1);
    int fragmentsToCopy = RangedRandomNumber(1, genome->size - pos);
    int newSize = genome->size + fragmentsToCopy;
    int newPos = RangedRandomNumber(0, genome->size - 1);
    uint16_t * fragment = (uint16_t*)malloc(fragmentsToCopy * sizeof(uint16_t));
    for (int i = pos, j = 0; j < fragmentsToCopy; i++,j++) {
        fragment[j] = genome->genes[i];
    }
    uint16_t* buffer = (uint16_t*)malloc(genome->size * sizeof(uint16_t));
    for (int i = 0; i < genome->size; i++) {
        buffer[i] = genome->genes[i];
    }
    genome->size = newSize;
    free(genome->genes);
    genome->genes = (uint16_t*)malloc(genome->size * sizeof(uint16_t));
    for (int i = 0; i < newPos; i++) {
        genome->genes[i] = buffer[i];
    }
    for (int i = newPos, j = 0; j < fragmentsToCopy; i++, j++) {
        genome->genes[i] = fragment[j];
    }
    for (int i = newPos + fragmentsToCopy, j = newPos; i < genome->size; i++, j++) {
        genome->genes[i] = buffer[j];
    }
    free(buffer);
    free(fragment);
}

void RandomFragmentMove(struct uint16_genome * genome) {
    int pos = RangedRandomNumber(0, genome->size - 1);
    int fragmentsToMove = RangedRandomNumber(1, genome->size - pos);
    int newSize = genome->size - fragmentsToMove;
    uint16_t * fragment = (uint16_t*)malloc(fragmentsToMove * sizeof(uint16_t));
    for (int i = pos, j = 0; j < fragmentsToMove; i++,j++) {
        fragment[j] = genome->genes[i];
    }
    uint16_t* buffer = (uint16_t*)malloc(newSize * sizeof(uint16_t));
    for (int i = 0; i < pos; i++) {
        buffer[i] = genome->genes[i];
    }
    for (int i = pos + fragmentsToMove, j = pos; j < newSize; i++, j++) {
        buffer[j] = genome->genes[i];
    }
    int newPos = RangedRandomNumber(0, newSize - 1);

    for (int i = 0; i < newPos; i++) {
        genome->genes[i] = buffer[i];
    }
    for (int i = newPos, j = 0; j < fragmentsToMove; i++, j++) {
        genome->genes[i] = fragment[j];
    }
    for (int i = newPos + fragmentsToMove, j = newPos; j < newSize; i++,j++) {
        genome->genes[i] = buffer[j];
    }
    free(fragment);
    free(buffer);
}

void RandomFragmentDuplicate (struct uint16_genome * genome) {
    int pos = RangedRandomNumber(0, genome->size - 1);
    int fragmentsToDuplicate = RangedRandomNumber(1, genome->size - pos);
    int newSize = genome->size + fragmentsToDuplicate;
    uint16_t* buffer = (uint16_t*)malloc(newSize * sizeof(uint16_t));
    for (int i = 0; i < pos + fragmentsToDuplicate; i++) {
        buffer[i] = genome->genes[i];
    }
    for (int i = pos + fragmentsToDuplicate, j = pos; i < newSize; i++, j++) {
        buffer[i] = genome->genes[j];
    }
    genome->size = newSize;
    free(genome->genes);
    genome->genes = (uint16_t*)malloc(genome->size * sizeof(uint16_t));
    for (int i = 0; i < genome->size; i++) {
        genome->genes[i] = buffer[i];
    }  
    free(buffer);
}

void RandomFragmentInsetrion(struct uint16_genome * genome) {
    int insert = RangedRandomNumber(0, genome->size - 1);
    int fragmentsToInsert = RangedRandomNumber(1, MAX_FRAGMENTS_TO_INSERT);
    int newSize = genome->size + fragmentsToInsert;
    uint16_t * fragment = (uint16_t*)malloc(fragmentsToInsert * sizeof(uint16_t));
    for (int i = 0; i < fragmentsToInsert; i++) {
        fragment[i] = RangedRandomNumber(0, MAX_UINT16);
    }
    uint16_t* buffer = (uint16_t*)malloc(newSize * sizeof(uint16_t));
    for (int i = 0; i < insert; i++) {
        buffer[i] = genome->genes[i];
    }
    for (int i = insert, j = 0; i < insert + fragmentsToInsert; i++, j++) {
        buffer[i] = fragment[j];
    }
    for (int i = insert + fragmentsToInsert, j = insert; i < newSize; i++, j++) {
        buffer[i] = genome->genes[j];
    }

    genome->size = newSize;
    free(genome->genes);
    genome->genes = (uint16_t*)malloc(genome->size * sizeof(uint16_t));
    for (int i = 0; i < genome->size; i++) {
        genome->genes[i] = buffer[i];
    }  
    free(fragment);
    free(buffer);
}

void RandomFragmentDeletion(struct uint16_genome * genome) {
    int remove = RangedRandomNumber(0, genome->size - 1);
    int fragmentsToRemove = RangedRandomNumber(1, genome->size - remove);
    int newSize = genome->size - fragmentsToRemove;
    uint16_t* buffer = (uint16_t*)malloc(newSize * sizeof(uint16_t));

    for (int i = 0; i < remove; i++) {
        buffer[i] = genome->genes[i];
    }
    for (int i = remove + fragmentsToRemove, j = remove; i < genome->size; i++, j++) {
        buffer[j] = genome->genes[i];
    }
    genome->size = newSize;
    free(genome->genes);
    genome->genes = (uint16_t *)malloc(newSize * sizeof(uint16_t));
    for (int i = 0; i < newSize; i++) {
        genome->genes[i] = buffer[i];
    }
    free(buffer);
}

void ChangeRandomBits(struct uint16_genome * genome) {
    for (int i = 0; i < genome->size; i++) {
        uint16_t uint16 = 1;
        if (RangedRandomNumber(0,100) < CHANGE_BITS_PROPABILITY) {
            genome->genes[i] ^= (uint16 << RangedRandomNumber(0,15));
        }
    }
 }
