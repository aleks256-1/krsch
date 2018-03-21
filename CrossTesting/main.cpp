#include <fstream>
#include <malloc.h>
#include "creature.h"
#include <iostream>
#include "main.h"
#include "rdev.h"
#include "genome.h"
#include "embriogenesis.h"
#include "bmpgen.h"
#include "cross.h"
#include <bitset>
#include <iostream>
#include <vector>

using namespace std;



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

    cross(genome, genome1, genome2);

    

 

    printGenome(genome);
    cout << "----------------\n";
    printGenome(genome1);
    cout << "----------------\n";
    printGenome(genome2);

    




    free(genome2);
    free(genome1);
    free(genome);


    return 0;
}
