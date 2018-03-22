#include <iostream>
#include <vector>
#include "rdev.h"
#include "genome.h"

using namespace std;

void crossGenome_uint16(uint16_genome * genome1, uint16_genome * genome2, uint16_genome * genome3) {
    vector<vector<int>> matrixOfEntries;
    matrixOfEntries.resize(genome1->size + 1);
    for(int i = 0; i <= static_cast<int>(genome1->size); i++)
        matrixOfEntries[i].resize(genome2->size + 1);
    for(int i = static_cast<int>(genome1->size) - 1; i >= 0; i--) {
        for(int j = static_cast<int>(genome2->size) - 1; j >= 0; j--) {
            if(genome1->genes[i] == genome2->genes[j]) {
                matrixOfEntries[i][j] = 1 + matrixOfEntries[i+1][j+1];
            } else {
                matrixOfEntries[i][j] = max(matrixOfEntries[i+1][j], matrixOfEntries[i][j+1]);                
            }
        }
    }
    vector<uint16_t> res, fragment1, fragment2;
    fragment1.clear();
    fragment2.clear();
    res.clear();
    for(int i = 0, j = 0; i < static_cast<int>(genome1->size) || j < static_cast<int>(genome2->size); ) {
        if(i < static_cast<int>(genome1->size) && j < static_cast<int>(genome2->size) && genome1->genes[i] == genome2->genes[j]) {
            if (fragment2.size()==0) {
                for (int k = 0; k < fragment1.size(); k++) {
                    res.push_back(fragment1[k]);
                }
            } else if (fragment1.size() == 0) {
                for (int k = 0; k < fragment2.size(); k++) {
                    res.push_back(fragment2[k]);
                }
            } else  if (RangedRandomNumber(0,50) < 25) {
                for (int k = 0; k < fragment1.size(); k++) {
                    res.push_back(fragment1[k]);
                }
            } else {
                for (int k = 0; k < fragment2.size(); k++) {
                    res.push_back(fragment2[k]);
                }
            }
            fragment1.clear();
            fragment2.clear();
            res.push_back(genome1->genes[i]);
            i++;
            j++;
        } else {
            if(i < static_cast<int>(genome1->size) && matrixOfEntries[i][j] == matrixOfEntries[i+1][j]) {
                fragment1.push_back(genome1->genes[i]);
                i++;
            } else if (j < static_cast<int>(genome2->size)) {
                fragment2.push_back(genome2->genes[j]);                
                j++;
            }          
        }
    }
    if (fragment2.size()==0) {
        for (int k = 0; k < fragment1.size(); k++) {
            res.push_back(fragment1[k]);
        }
    } else if (fragment1.size() == 0) {
        for (int k = 0; k < fragment2.size(); k++) {
            res.push_back(fragment2[k]);
        }
    } else  if (RangedRandomNumber(0,50) < 25) {
        for (int k = 0; k < fragment1.size(); k++) {
            res.push_back(fragment1[k]);
        }
    } else {
        for (int k = 0; k < fragment2.size(); k++) {
            res.push_back(fragment2[k]);
        }
    }
    genome3->size = res.size();
    genome3->genes = (uint16_t*)malloc(genome3->size * sizeof(uint16_t));
    for (int i = 0; i < genome3->size; i++) {
        genome3->genes[i] = res[i];
    }

}