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
    char ip[] = "195.19.58.174"; // Server IP
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
    ssize_t rec;
    int res, i, step;
    struct uint16_genome * nextGeneration;
    struct genome * genome;
    struct creature * creature = (struct creature *)malloc(sizeof(struct creature));
    struct matrix * matrix;
    nextGeneration = (struct uint16_genome *)malloc(sizeof(struct uint16_genome));
    genome = (struct genome *)malloc(sizeof(struct genome));
    init_blur_matrix(&matrix);
    struct creature * MyCreature;
    MyCreature = (struct creature*)malloc(sizeof(struct creature));
    initCreature(MyCreature);
    for (int i = 0; i < 128; i++) {
        rec = 0;
        do {
            int res = recv(client, &buffer[rec], bufsize-rec, 0);
            rec+=res;
        } while (rec<bufsize);   
        for (int j = 0; j < N * N; j++) {
            MyCreature->cells[j].v[i] = MyCreature->cells[j].dv[i] = atoi(buffer);
        }
    }
    int genomeNumber;
    unsigned char * image;
    //    readBitmapImage((unsigned char //24*)image, 225, 225, (char *)("IdealImage.bmp"));
    image = readBMP((char*)"ideal.bmp");
    generateBitmapImage(image, IMAGE_HEIGHT, IMAGE_WIDTH, (char*)"generated.bmp");
    write(client, buffer, bufsize);
    int count = 0;
    do {
        rec = 0;
        do {
            int res = recv(client, &buffer[rec], bufsize-rec, 0);
            rec+=res;
        } while (rec<bufsize);
        if (atoi(buffer) == 123456789) {
            rec = 0;
            cout << "//////////////////////////////////////\n             done generation # " << count << "\n//////////////////////////////////////\n";
            count ++;
            do {
                int res = send(client, &buffer[rec], bufsize-rec, 0);
                rec+=res;
            } while (rec<bufsize);
        } else {
            nextGeneration->size = atoi(buffer);
            rec = 0;
            do {
                res = recv(client, &buffer[rec], bufsize - rec, 0);
                rec += res;
            } while (rec < bufsize);
            genomeNumber = atoi(buffer);
            if(nextGeneration->genes) {
                free(nextGeneration->genes);
            } 
            nextGeneration->genes = (uint16_t*)malloc(nextGeneration->size * sizeof(uint16_t));
            for (int j = 0; j < nextGeneration->size; j++) {
                rec = 0;
                do {
                    res = recv(client, &buffer[rec], bufsize - rec, 0);
                    rec += res;
                } while (rec < bufsize);
                nextGeneration->genes[j] = atoi(buffer);
            } 
            nextGeneration->similarity = 0.0;      

            cout << "Growing up creature # " << genomeNumber << "\n";
            if (creature) {
                free(creature->cells);
                free(creature);
            }
            creature = (struct creature*)malloc(sizeof(struct creature));
            initCreature(creature);
            int size = creature->n * creature->n;
            for (int j = 0; j < size; j++) {
                for (int k = 0; k < SUBSTANCE_LENGTH; k++) {
                    creature->cells[j].v[k] = creature->cells[j].dv[k] = MyCreature->cells[j].v[k];
                }
            }
            if(genome) {
                free(genome->genes);
                free(genome);
            }
            genome = (struct genome*)malloc(sizeof(struct genome));
            uint16genome_to_genome(nextGeneration, genome);
            step = 0;
            while(creature->n < MAX_CREATURE_SIZE) {
//                cout << "Size: " << creature->n << " x " << creature->n << '\n';
                calculateDvForCells_v2(creature, genome);
                applyDvForCells(creature);
                diff_v2(creature, matrix);
                applyDiff(creature);
                if (step % 2 == 0) {
                    creature = grow(creature);
                }
                step++;
            }

            float r = 0, g = 0, b = 0;
            size = creature->n * creature->n;
            for (int j = 0; j < creature->n; j++) {
                for (int k = 0; k < creature->n; k++) {
                    r += abs(creature->cells[j*creature->n+k].v[RED_COMPONENT] - (float)image[3 * (j * IMAGE_WIDTH + k)]);
                    g += abs(creature->cells[j*creature->n+k].v[GREEN_COMPONENT] - (float)image[3 * (j * IMAGE_WIDTH + k) + 1]);
                    b += abs(creature->cells[j*creature->n+k].v[BLUE_COMPONENT] - (float)image[3 * (j * IMAGE_WIDTH + k) + 2]);
                }
            }
            genome->similarity = (r + g + b) / size;
//            cout << " similarity is " << genome->similarity << '\n';
            strcpy(buffer, to_string(genomeNumber).c_str());
            rec = 0;
            do {
                int res = send(client, &buffer[rec], bufsize-rec, 0);
                rec+=res;
            } while (rec<bufsize);
            strcpy(buffer, to_string(genome->similarity).c_str());
            rec = 0;
            do {
                int res = send(client, &buffer[rec], bufsize-rec, 0);
                rec+=res;
            } while (rec<bufsize);    
//            char filename[FILENAME_MAX];
//            sprintf(filename, "c%d.bmp", genomeNumber);
//            createImage(creature, filename);

        }
    } while (!isExit);

    free(nextGeneration);
    free(genome);

 /* ---------------- CLOSE CALL ------------- */
    cout << "\nConnection terminated.\n";
    close(client);
    return 0;
}
