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
#include "genome.h"
#include "embriogenesis.h"
#include "bmpgen.h"
#include <cmath>

int IMAGE_WIDTH;
int IMAGE_HEIGHT;

unsigned char* readBMP(char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    IMAGE_WIDTH = *(int*)&info[18];
    IMAGE_HEIGHT = *(int*)&info[22];

    int size = 3 * IMAGE_WIDTH * IMAGE_HEIGHT;
    cout << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << '\n';
    unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
    fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);

    for(i = 0; i < size; i += 3)
    {
            unsigned char tmp = data[i];
            data[i] = data[i+2];
            data[i+2] = tmp;
    }

    return data;
}

using namespace std;

int main(int argc, char ** argv) {
    /* -------------- INITIALIZING VARIABLES -------------- */
    int client; // socket file descriptors
    int portNum = 8080; // port number (same that server)
    int bufsize = 65536; // buffer size
    char buffer[bufsize]; // buffer to transmit
    char ip[] = "127.0.0.1"; // Server IP
    bool isExit = false; // var fo continue infinitly
     /* Structure describing an Internet socket address. */
    struct sockaddr_in server_addr;
    cout << "\n- Starting client..." << endl;
     /* ---------- ESTABLISHING SOCKET CONNECTION ----------*/
    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        cout << "\n-Error establishing socket..." << endl;
        exit(-1);
    }
    cout << "\n- Socket client has been created..." << endl;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);
 /* ---------- CONNECTING THE SOCKET ---------- */
    if (connect(client, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        cout << "- Connection to the server port number: " << portNum << endl;
    cout << "- Awaiting confirmation from the server..." << endl; //line 40
 // recive the welcome message from server
    cout << "- Connection confirmed, you are good to go!" << endl;


    struct uint16_genome * nextGeneration[NEXT_GENERATION_POPULATION];
    struct genome * genome[NEXT_GENERATION_POPULATION];
    struct creature * creature[NEXT_GENERATION_POPULATION];
    struct matrix * matrix;
    for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
        nextGeneration[i] = (struct uint16_genome *)malloc(sizeof(struct uint16_genome));
        genome[i] = (struct genome *)malloc(sizeof(struct genome));
    }
    init_blur_matrix(&matrix);
    struct creature * MyCreature;
    MyCreature = (struct creature*)malloc(sizeof(struct creature));
    initCreature(MyCreature);
    ssize_t rec;
    int res, i, step;


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////      IDEAL CREATURE      //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char * image;
//    readBitmapImage((unsigned char *)image, 225, 225, (char *)("IdealImage.bmp"));
    image = readBMP((char*)"ideal.bmp");
    generateBitmapImage(image, IMAGE_HEIGHT, IMAGE_WIDTH, (char*)"generated.bmp");
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////    LOOP WITH SERVER    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
    int counters = 0;
    do {
//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////      GET POPULATION       //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
        cout << 1 << '\n';
        for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
            rec = 0;
            do {
                res = recv(client, &buffer[rec], bufsize - rec, 0);
                rec += res;
            } while (rec < bufsize);
            nextGeneration[i]->size = atoi(buffer);
            if(nextGeneration[i]->genes) {
                free(nextGeneration[i]->genes);
            }
            nextGeneration[i]->genes = (uint16_t*)malloc(nextGeneration[i]->size * sizeof(uint16_t));
            for (int j = 0; j < nextGeneration[i]->size; j++) {
                rec = 0;
                do {
                    res = recv(client, &buffer[rec], bufsize - rec, 0);
                    rec += res;
                } while (rec < bufsize);
                nextGeneration[i]->genes[j] = atoi(buffer);
            } 
            nextGeneration[i]->similarity = 0.0;      
        }
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////        GROWING UP CREATURES        /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
        cout << "Growing up creatures \n";
        for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
            if (creature[i]) {
                free(creature[i]->cells);
                free(creature[i]);
            }
            creature[i] = (struct creature*)malloc(sizeof(struct creature));
            initCreature(creature[i]);
        }
        for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
            int size = creature[i]->n * creature[i]->n;
            for (int j = 0; j < size; j++) {
                for (int k = 0; k < SUBSTANCE_LENGTH; k++) {
                    creature[i]->cells[j].v[k] = creature[i]->cells[j].dv[k] = MyCreature->cells[j].v[k];
                }
            }
        }
        for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
            cout << "##########################   Creature " << i << "   ##########################\n";
            if(genome[i]) {
                free(genome[i]->genes);
                free(genome[i]);
            }
            genome[i] = (struct genome*)malloc(sizeof(struct genome));
            uint16genome_to_genome(nextGeneration[i], genome[i]);
            step = 0;
            while(creature[i]->n < MAX_CREATURE_SIZE) {
                cout << "Size: " << creature[i]->n << " x " << creature[i]->n << '\n';
                calculateDvForCells_v2(creature[i], genome[i]);
                applyDvForCells(creature[i]);
                diff_v2(creature[i], matrix);
                applyDiff(creature[i]);
                if (step % 2 == 0) {
                    cout << "grow\n";
                    creature[i] = grow(creature[i]);
                    cout << "end grow\n";
                }
                step++;
            }
        }
        cout << "Done\n";
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
        
//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////        CALCULATING SIMILARITY        ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////        
        for(int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
            float r = 0, g = 0, b = 0;
            int size = creature[i]->n * creature[i]->n;
            for (int j = 0; j < creature[i]->n; j++) {
                for (int k = 0; k < creature[i]->n; k++) {
                    r += abs(creature[i]->cells[j*creature[i]->n+k].v[RED_COMPONENT] - (float)image[3 * (j * IMAGE_WIDTH + k)]);
                    g += abs(creature[i]->cells[j*creature[i]->n+k].v[GREEN_COMPONENT] - (float)image[3 * (j * IMAGE_WIDTH + k) + 1]);
                    b += abs(creature[i]->cells[j*creature[i]->n+k].v[BLUE_COMPONENT] - (float)image[3 * (j * IMAGE_WIDTH + k) + 2]);
                }
            }
            genome[i]->similarity = (r + g + b) / size;
            cout << " similarity of " << i << " is " << genome[i]->similarity << '\n';
        }
////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////// 

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////         SENDING MOST SIMILAR         ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < START_POPULATION; i++) {
            int mostSimilar = 0; 
            int val = 1000;
            for (int j = 0; j < NEXT_GENERATION_POPULATION; j++) {
                if (genome[mostSimilar]->similarity >= 0 && genome[j]->similarity < val) {
                    val = genome[j]->similarity;
                    mostSimilar = j;
                }
            }
            
            strcpy(buffer, to_string(mostSimilar).c_str());
            rec = 0;
            do {
                int res = send(client, &buffer[rec], bufsize-rec, 0);
                rec+=res;
            } while (rec<bufsize);   
            genome[mostSimilar]->similarity = -1;
            cout << "genome # " << mostSimilar << " is most similar\n";
            
            char filename[FILENAME_MAX];
            sprintf(filename, "c%d.bmp", i);
            createImage(creature[mostSimilar], filename);
            genome[mostSimilar]->similarity = 1001;
        }
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//        send(client, buffer, bufsize, 0);
        counters++;
    } while (!isExit);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  FREE MEMORY  ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
        if (nextGeneration[i]) {
            free(nextGeneration[i]->genes);
            free(nextGeneration[i]);
        }
        if (genome[i]) {
            free(genome[i]->genes);
            free(genome[i]);
        }
        if (creature[i]) {
            free(creature[i]->cells);
            free(creature[i]);
        }
    }
    free(MyCreature->cells);
    free(MyCreature);
    free(matrix);
    free(image);
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


 /* ---------------- CLOSE CALL ------------- */
    cout << "\nConnection terminated.\n";
    close(client);
    return 0;
}
