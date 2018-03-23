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
    }
    init_blur_matrix(&matrix);
    struct creature * MyCreature = (struct creature *)malloc(sizeof(struct creature));
    initCreature(&MyCreature);
    ssize_t rec;
    int res, i, step;


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////      IDEAL CREATURE      //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char image[512][512][bytesPerPixel];
    readBitmapImage((unsigned char *)image, 225, 225, (char *)("IdealImage.bmp"));
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            cout << (int)image[i][j][0] << ' ' << (int)image[i][j][1] << ' ' << (int)image[i][j][2] << '\n';
        }
    }
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
            initCreature(&creature[i]);
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
            float r = 0, g = 0, b = 0, sumr = 0;
            int size = creature[i]->n * creature[i]->n;
            for (int j = 0; j < creature[i]->n; j++) {
                for (int k = 0; k < creature[i]->n; k++) {
                    sumr += creature[i]->cells[j*creature[i]->n+k].v[RED_COMPONENT];
                    r += abs(creature[i]->cells[j*creature[i]->n+k].v[RED_COMPONENT] - image[j][k][0]);
                    g += abs(creature[i]->cells[j*creature[i]->n+k].v[GREEN_COMPONENT] - image[j][k][1]);
                    b += abs(creature[i]->cells[j*creature[i]->n+k].v[BLUE_COMPONENT] - image[j][k][2]);
                }
            }
            cout << r/size << ' ' << g/size << ' ' << b/size << '\n';
            genome[i]->similarity = (r + g + b) / size;
            cout << sumr << " similarity of " << i << " is " << genome[i]->similarity << '\n';
        }
////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////// 

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////         SENDING MOST SIMILAR         ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < START_POPULATION; i++) {
            int mostSimilar = 0;
            for (int j = 0; j < NEXT_GENERATION_POPULATION; j++) {  
                if (genome[j]->similarity <= genome[mostSimilar]->similarity && genome[j]->similarity > 0) {
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
            
        }
        for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
            free(creature[i]->cells);
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
        free(creature[i]);
        free(genome[i]);
    }
    free(MyCreature);
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


 /* ---------------- CLOSE CALL ------------- */
    cout << "\nConnection terminated.\n";
    close(client);
    return 0;
}
