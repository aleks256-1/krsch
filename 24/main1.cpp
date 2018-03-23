#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <iostream>
#include <bitset>
#include <unistd.h>


#include "cross.h"
#include "mutations.h"
#include "creature.h"
#include "main.h"
#include "rdev.h"
#include "genome.h"
#include "embriogenesis.h"
#include "bmpgen.h"

using namespace std;


int main() {
    unsigned char image[512][512][bytesPerPixel];
    readBitmapImage((unsigned char *)image, 225, 225, (char *)("IdealImage.bmp"));
    for (int i = 0; i < 225; i++) {
        for (int j = 0; j < 225; j++) {
            cout << (int)image[i][j][0] << ' ' << (int)image[i][j][1] << ' ' << (int)image[i][j][2] << '\n';
        }
    }

    generateBitmapImage((unsigned char *)image, 225, 225, (char*)("I.bmp"));

    return 0;
}