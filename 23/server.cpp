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

void copy_uint16_genome(struct uint16_genome * genome1, struct uint16_genome * genome2) {
    free(genome1->genes);
    genome1->size = genome2->size;
    genome1->genes = (uint16_t *)malloc(genome1->size * sizeof(uint16_t));
    for (int i = 0; i < genome1->size; i++) {
        genome1[i] = genome2[i];
    }
}


int main(int argc, char ** argv) {
 /* -------------- INITIALIZING VARIABLES -------------- */
    int server, client; // socket file descriptors
    int portNum = 8080; // port number
    int bufsize = 65536; // buffer size
    char buffer[bufsize]; // buffer to transmit
    bool isExit = false; // var fo continue infinitly
    struct sockaddr_in server_addr;
    socklen_t size;
    cout << "\n- Starting server..." << endl;
 /* ---------- ESTABLISHING SOCKET CONNECTION ----------*/
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        cout << "Error establishing socket ..." << endl;
        exit(-1);
    }
    cout << "- Socket server has been created..." << endl;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portNum);
    int yes = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
        exit(1);
    }
 /* ---------------- BINDING THE SOCKET --------------- */
    if ((bind(server, (struct sockaddr*) &server_addr, sizeof(server_addr))) < 0) {
        cout << "- Error binding connection, the socket has already been established..." << endl;
        exit(-1);
    }
 /* ------------------ LISTENING CALL ----------------- */
    size = sizeof(server_addr);
    cout << "- Looking for clients..." << endl;
    listen(server, 1);
 /* ------------------- ACCEPT CALL ------------------ */
    client = accept(server, (struct sockaddr *) &server_addr, &size);
    if (client < 0)
        cout << "- Error on accepting..." << endl;
//Строим первое поколение
    struct uint16_genome * firstGeneration[START_POPULATION], * nextGeneration[NEXT_GENERATION_POPULATION];
    struct uint16_genome * idealGenome;
//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////      IDEAL GENOME      ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////    
    idealGenome = (struct uint16_genome *)malloc(sizeof(struct uint16_genome));
    initRandGenome_uint16(idealGenome);
    strcpy(buffer, to_string(idealGenome->size).c_str());
    send(client, buffer, bufsize, 0); 
    for (int j = 0; j < idealGenome->size; j++) {
        strcpy(buffer, to_string(idealGenome->genes[j]).c_str());
        send(client, buffer, bufsize, 0); 
    }
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////  INITIALIZING FIRST GENERATION ARRAYS  ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < START_POPULATION; i++) {
        firstGeneration[i] = (struct uint16_genome *)malloc(sizeof(struct uint16_genome));
        initRandGenome_uint16(firstGeneration[i]);
    }                                                  
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    ssize_t rec;
    int count = 0, res;
    while (client > 0) {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////    LOOP WITH CLIENT    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
        do {
//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////  INITIALIZING NEXT GENERATION ARRAYS  ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
            for (int i = 0; i < START_POPULATION; i++) {
                nextGeneration[i] = (struct uint16_genome *)malloc(sizeof(struct uint16_genome));
                copyGenome_uint16(firstGeneration[i], nextGeneration[i]);
            }
            for (int i = START_POPULATION; i < NEXT_GENERATION_POPULATION; i++) {
                nextGeneration[i] = (struct uint16_genome *)malloc(sizeof(struct uint16_genome)); 
                crossGenome_uint16(firstGeneration[RangedRandomNumber(0, START_POPULATION - 1)], 
                                    firstGeneration[RangedRandomNumber(0, START_POPULATION - 1)],
                                    nextGeneration[i]);
            }
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
            
//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  MUTATIONS  ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
            int numberOfMutations = RangedRandomNumber(NEXT_GENERATION_POPULATION/2, NEXT_GENERATION_POPULATION); 
            for (int i = 0; i < numberOfMutations; i++) {                                           
                int mutation = RangedRandomNumber(0,100);                                          
                int creature = RangedRandomNumber(0, NEXT_GENERATION_POPULATION - 1);                        
                if (mutation < 13) {                     
                    ChangeRandomBits(nextGeneration[creature]);                           
                } else if (mutation < 25) {    
                    RandomFragmentDeletion(nextGeneration[creature]);                          
                } else if (mutation < 38) {                                                         
                    RandomFragmentInsetrion(nextGeneration[creature]);                         
                } else if (mutation < 50) {    
                    RandomFragmentDuplicate(nextGeneration[creature]);                    
                } else if (mutation < 63) {                                    
                    RandomFragmentMove(nextGeneration[creature]);                            
                } else if (mutation < 75) {  
                    RandomFragmentCopy(nextGeneration[creature]);                            
                } else if (mutation < 88) {                                                       
                    int secondCreature = RangedRandomNumber(0, NEXT_GENERATION_POPULATION - 1);
                    ExchangeDnaFragments(nextGeneration[creature], nextGeneration[secondCreature]);
                } else if (mutation < 100) { 
                    RandomFragmentReverse(nextGeneration[creature]);
                }                                                                                   
            }                                                                                      
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////          SEND GENOME TO CLIENT         //////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
            for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
                strcpy(buffer, to_string(nextGeneration[i]->size).c_str());
                send(client, buffer, bufsize, 0); 
                for (int j = 0; j < nextGeneration[i]->size; j++) {
                    strcpy(buffer, to_string(nextGeneration[i]->genes[j]).c_str());
                    send(client, buffer, bufsize, 0); 
                }       
            }
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////          RECEIVE SURVIVORS FROM CLIENT         //////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
            for(int i = 0; i < START_POPULATION; i++) {
                rec = 0;
                do {
                    res = recv(client, &buffer[rec], bufsize - rec, 0);
                    rec += res;
                } while (rec < bufsize);
                copy_uint16_genome(firstGeneration[i], nextGeneration[atoi(buffer)]);
            }                
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
            cout<<"done #"<<count<<endl;
            count++;
        } while (!isExit);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 /* ---------------- CLOSE CALL ------------- */
        cout << "\n\n=> Connection terminated with IP " << inet_ntoa(server_addr.sin_addr);
        close(client);
        cout << "\nGoodbye..." << endl;
        exit(1);
    }

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  FREE MEMORY  ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
        free(nextGeneration[i]);
    }
    for (int i = 0; i < START_POPULATION; i++) {
        free(firstGeneration[i]);
    }
    free(idealGenome);
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
 /* ---------------- CLOSE CALL ------------- */
    close(server);
    return 0;
}