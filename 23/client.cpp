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
#include <cmath>

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
        initCreature(&creature[i]);
    }
    init_blur_matrix(&matrix);
    for (int i = 1; i < NEXT_GENERATION_POPULATION; i++) {
        int size = creature[i]->n * creature[i]->n;
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < SUBSTANCE_LENGTH; k++) {
                creature[i]->cells[j].v[k] = creature[i]->cells[j].dv[k] = creature[0]->cells[j].v[k];
            }
        }
    }

    ssize_t rec;
    int res, i, step;

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////      IDEAL GENOME      ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////    
    struct uint16_genome * ideal_uint16_genome;
    struct genome * idealGenome;
    ideal_uint16_genome = (struct uint16_genome *)malloc(sizeof(struct uint16_genome));
    idealGenome = (struct genome *)malloc(sizeof(struct genome));
    rec = 0;
    do {
        res = recv(client, &buffer[rec], bufsize - rec, 0);
        rec += res;
    } while (rec < bufsize);
    ideal_uint16_genome->size = atoi(buffer);
    ideal_uint16_genome->genes = (uint16_t*)malloc(ideal_uint16_genome->size * sizeof(uint16_t));
    for (int j = 0; j < ideal_uint16_genome->size; j++) {
        rec = 0;
        do {
            res = recv(client, &buffer[rec], bufsize - rec, 0);
            rec += res;
        } while (rec < bufsize);
        ideal_uint16_genome->genes[j] = atoi(buffer);
    } 
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////      IDEAL CREATURE      //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    struct creature * idealCreature;
    initCreature(&idealCreature);
    for (int i = 1; i < NEXT_GENERATION_POPULATION; i++) {
        int size = idealCreature->n * idealCreature->n;
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < SUBSTANCE_LENGTH; k++) {
                idealCreature->cells[j].v[k] = idealCreature->cells[j].dv[k] = creature[0]->cells[j].v[k];
            }
        }
    } 
    char* imageNames[FILENAME_MAX];
    imageNames[0] = (char*)"1.bmp";
    imageNames[1] = (char*)"2.bmp";
    imageNames[2] = (char*)"3.bmp";
    imageNames[3] = (char*)"4.bmp";
    imageNames[4] = (char*)"5.bmp";
    imageNames[5] = (char*)"6.bmp";
    imageNames[6] = (char*)"7.bmp";
    imageNames[7] = (char*)"8.bmp";
    imageNames[8] = (char*)"9.bmp";
    imageNames[9] = (char*)"10.bmp";
    imageNames[10] = (char*)"11.bmp";
    imageNames[11] = (char*)"12.bmp";
    imageNames[12] = (char*)"13.bmp";
    imageNames[13] = (char*)"14.bmp";
    imageNames[14] = (char*)"15.bmp";
    imageNames[15] = (char*)"16.bmp";
    imageNames[16] = (char*)"17.bmp";
    imageNames[17] = (char*)"18.bmp";
    step = 0;
    i = 0;
    uint16genome_to_genome(ideal_uint16_genome, idealGenome);
    while(idealCreature->n < MAX_CREATURE_SIZE) {
        calculateDvForCells_v2(idealCreature, idealGenome);
        applyDvForCells(idealCreature);
        diff_v2(idealCreature, matrix);
        applyDiff(idealCreature);
        if (step % 2 == 0) {
            createImage(idealCreature, (char*)imageNames[i]);
            i++;
            idealCreature = grow(idealCreature);
        }
        step++;
    }
    createImage(idealCreature, (char*)imageNames[i]);
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////    LOOP WITH SERVER    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
    do {
//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////      GET POPULATION       //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
            rec = 0;
            do {
                res = recv(client, &buffer[rec], bufsize - rec, 0);
                rec += res;
            } while (rec < bufsize);
            nextGeneration[i]->size = atoi(buffer);
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
            cout << "##########################   Creature " << i << "   ##########################\n";
            uint16genome_to_genome(nextGeneration[i], genome[i]);
            step = 0;
            while(creature[i]->n < MAX_CREATURE_SIZE) {
                cout << "Size: " << creature[i]->n << " x " << creature[i]->n << '\n';
                calculateDvForCells_v2(creature[i], genome[i]);
                applyDvForCells(creature[i]);
                diff_v2(creature[i], matrix);
                applyDiff(creature[i]);
                if (step % 2 == 0) {
                    creature[i] = grow(creature[i]);
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
            int r = 0, g = 0, b = 0;
            int size = creature[i]->n * creature[i]->n;
            for (int j = 0; j < size; j++) {
                r += abs(creature[i]->cells[j].v[RED_COMPONENT] - idealCreature->cells[j].v[RED_COMPONENT]);
                g += abs(creature[i]->cells[j].v[GREEN_COMPONENT] - idealCreature->cells[j].v[GREEN_COMPONENT]);
                b += abs(creature[i]->cells[j].v[BLUE_COMPONENT] - idealCreature->cells[j].v[BLUE_COMPONENT]);
            }
            genome[i]->similarity = (float)(r + g + b);
            cout << "similarity of " << i << " is " << genome[i]->similarity << '\n';
        }
////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////// 

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////         SENDING MOST SIMILAR         ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < START_POPULATION; i++) {
            int val = 255;
            int mostSimilar = -1;
            for (int j = 0; j < NEXT_GENERATION_POPULATION; j++) {  
                if (genome[j]->similarity < val) {
                    val = genome[j]->similarity;
                    mostSimilar = j;
                }
            }
            strcpy(buffer, to_string(mostSimilar).c_str());
            send(client, buffer, bufsize, 0);   
            genome[mostSimilar]->similarity = -1;
            cout << "genome # " << mostSimilar << " is most similar\n";
        }
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//        send(client, buffer, bufsize, 0);
    } while (!isExit);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  FREE MEMORY  ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
        free(nextGeneration[i]);
        free(genome[i]);
    }
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


 /* ---------------- CLOSE CALL ------------- */
    cout << "\nConnection terminated.\n";
    close(client);
    return 0;
}