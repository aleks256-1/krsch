#ifndef CROSS_H
#define CROSS_H

#include <vector>

using namespace std;

vector<uint16_t> cross_uint16_Arrays(uint16_t * a, int aSize, uint16_t * b, int bSize);
struct genome * cross(struct genome * genome1, struct genome * genome2, struct genome * genome3);

#endif