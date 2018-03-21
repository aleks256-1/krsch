#include <iostream>
#include <vector>
#include "rdev.h"
#include "genome.h"

using namespace std;


vector<uint16_t> cross_uint16_Arrays(uint16_t * a, int aSize, uint16_t * b, int bSize) {
    vector<vector<int>> matrixOfEntries;
    matrixOfEntries.resize(aSize + 1);
    for(int i = 0; i <= static_cast<int>(aSize); i++)
        matrixOfEntries[i].resize(bSize + 1);
    for(int i = static_cast<int>(aSize) - 1; i >= 0; i--) {
        for(int j = static_cast<int>(bSize) - 1; j >= 0; j--) {
            if(a[i] == b[j]) {
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
    for(int i = 0, j = 0; i < static_cast<int>(aSize) || j < static_cast<int>(bSize); ) {
        if(i < static_cast<int>(aSize) && j < static_cast<int>(bSize) && a[i] == b[j]) {
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
            res.push_back(a[i]);
            i++;
            j++;
        } else {
            if(i < static_cast<int>(aSize) && matrixOfEntries[i][j] == matrixOfEntries[i+1][j]) {
                fragment1.push_back(a[i]);
                i++;
            } else if (j < static_cast<int>(bSize)) {
                fragment2.push_back(b[j]);                
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
    return res;
}



void cross(struct genome * genome1, struct genome * genome2, struct genome * genome3) {
    saveGenome(genome1);    
    FILE * fp;
    fp = fopen("genome.txt", "rb");
    fseek(fp,0,SEEK_END);
    int size = (ftell(fp))/2;
	rewind(fp);
	uint16_t* buffer = (uint16_t*)malloc(size * sizeof(uint16_t));
    fread(buffer, sizeof(uint16_t), size, fp);
    fclose(fp);

    saveGenome(genome1);
    fp = fopen("genome.txt", "rb");
    fseek(fp,0,SEEK_END);
    int size1 = (ftell(fp))/2;
	rewind(fp);
	uint16_t* buffer1 = (uint16_t*)malloc(size1 * sizeof(uint16_t));
    fread(buffer1, sizeof(uint16_t), size1, fp);
    fclose(fp);

    uint16_t* buffer2;
    vector<uint16_t> gen3 = cross_uint16_Arrays(buffer, size, buffer1, size1);
    buffer2 = (uint16_t*)malloc(gen3.size() * sizeof(uint16_t));
    for(int i = 0; i < gen3.size(); i++) {
        buffer2[i] = gen3[i];
    }

    fp = fopen("genome.txt", "wb");
    for(int i = 0; i < gen3.size(); i++) {
        fwrite(&buffer2[i], sizeof(uint16_t), 1, fp);
    }
    fclose(fp);

    loadGenome(genome3);


    free(buffer);
    free(buffer1);
    free(buffer2);
}