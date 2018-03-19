void crossParents(struct genome * first, struct genome * second, struct genome * newGenome) {
    vector<vector<int> > matrixOfEntries;
    int aSize = first->length;
    int bSize = second->length;
    matrixOfEntries.resize(aSize + 1);
    for(int i = 0; i <= static_cast<int>(aSize); i++)
        matrixOfEntries[i].resize(bSize + 1);
    for(int i = static_cast<int>(aSize) - 1; i >= 0; i--) {
        for(int j = static_cast<int>(bSize) - 1; j >= 0; j--) {
            if(isEqual(first->genes[i], second->genes[i])) {
                matrixOfEntries[i][j] = 1 + matrixOfEntries[i+1][j+1];
            } else {
                matrixOfEntries[i][j] = max(matrixOfEntries[i+1][j], matrixOfEntries[i][j+1]);                
            }
        }
    }
    gene fragment1, fragment2;
    fragment1.cond_length = 0;
    fragment1.cond = NULL;
    fragment1.oper_length = 0;
    fragment1.operons = NULL;
    fragment2.cond_length = 0;
    fragment2.cond = NULL;
    fragment2.oper_length = 0;
    fragment2.operons = NULL;
    for(int i = 0, j = 0; i < static_cast<int>(aSize) || j < static_cast<int>(bSize); ) {
        if(i < static_cast<int>(aSize) && j < static_cast<int>(bSize) && isEqual(first->genes[i], second->genes[j])) {
            if (fragment2.cond_length == 0 && fragment2.oper_length) {
                addGene(newGenome, fragment1);
            } else if (fragment1.cond_length == 0 && fragment1.oper_length) {
                addGene(newGenome, fragment2);
            } else  if (RangedRandomNumber(0,50) < 25) {
                addGene(newGenome, fragment1);
            } else {
                addGene(newGenome, fragment2);
            }
            fragment1.cond_length = 0;
            fragment1.cond = NULL;
            fragment1.oper_length = 0;
            fragment1.operons = NULL;
            fragment2.cond_length = 0;
            fragment2.cond = NULL;
            fragment2.oper_length = 0;
            fragment2.operons = NULL;
            addGene(newGenome, first->genes[i]);
            i++;
            j++;
        } else {
            if(i < static_cast<int>(aSize) && matrixOfEntries[i][j] == matrixOfEntries[i+1][j]) {
                addToFragment(fragment1, first->genes[i]);
                i++;
            } else if (j < static_cast<int>(bSize)) {
                addToFragment(fragment2, second->genes[j]);
                j++;
            }          
        }
    }
    if (fragment2.cond_length == 0 && fragment2.oper_length) {
        addGene(newGenome, fragment1);
    } else if (fragment1.cond_length == 0 && fragment1.oper_length) {
        addGene(newGenome, fragment2);
    } else  if (RangedRandomNumber(0,50) < 25) {
        addGene(newGenome, fragment1);
    } else {
        addGene(newGenome, fragment2);
    }
}
