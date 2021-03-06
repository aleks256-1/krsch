#include <fstream>
#include <malloc.h>
#include "creature.h"
#include <iostream>
#include "main.h"
#include "rdev.h"
#include "genome.h"
#include "embriogenesis.h"
#include "bmpgen.h"
#include <bitset>
#include <iostream>
#include <vector>

using namespace std;

vector<uint16_t> cross(uint16_t * a, int aSize, uint16_t * b, int bSize) {
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


int main() {
    struct genome * genome;
    struct genome * genome1;
    struct genome * genome2;
    genome = (struct genome*)malloc(sizeof(struct genome));
    genome1 = (struct genome*)malloc(sizeof(struct genome));
    genome2 = (struct genome*)malloc(sizeof(struct genome));

    initRandGenome(genome);
    initRandGenome(genome1);
    


/*    uint16_t buffer1[3], buffer2[5], *buffer3;
    buffer1[0] = 1;
    buffer1[1] = 3;
    buffer1[2] = 2;

    buffer2[0] = 1;
    buffer2[1] = 5;
    buffer2[2] = 2;
    buffer2[3] = 7;
    buffer2[4] = 8;

    buffer3 = cross(buffer1, 3, buffer2, 5);
*/
    
/*    ifstream file("genome.txt");
    string temp;
    getline(file, temp);
    cout << temp.length();
    cout << "--------------------------\n";
    bitset<16> x(temp[0]);
    cout << x << '\n';*/

    saveGenome(genome);    
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

    printGenome(genome);
    printGenome(genome1);


    uint16_t* buffer2;
    vector<uint16_t> gen3 = cross(buffer, size, buffer1, size1);
    buffer2 = (uint16_t*)malloc(gen3.size() * sizeof(uint16_t));
    for(int i = 0; i < gen3.size(); i++) {
        buffer2[i] = gen3[i];
    }



    free(buffer);
    free(buffer1);
    free(buffer2);

    free(genome2);
    free(genome1);
    free(genome);


    return 0;
}
