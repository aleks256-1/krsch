#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <iostream>
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

#define TRUE   1 
#define FALSE  0 
#define PORT 8080 

void copy_uint16_genome(struct uint16_genome * genome1, struct uint16_genome * genome2) {
    free(genome1);
    genome1 = (struct uint16_genome *)malloc(sizeof(struct uint16_genome));
    genome1->size = genome2->size;
    genome1->genes = (uint16_t *)malloc(genome1->size * sizeof(uint16_t));
    for (int i = 0; i < genome1->size; i++) {
        genome1[i] = genome2[i];
    }
}


int main(int argc , char *argv[])  
{  
    int opt = TRUE;  
    int master_socket , addrlen , new_socket , client_socket[30] , 
          max_clients = 30 , activity, i , valread , sd;  
    int max_sd;  
    struct sockaddr_in address;  
        
    int bufsize = 65536; // buffer size
    char buffer[bufsize]; // buffer to transmit
        
    //set of socket descriptors 
    fd_set readfds;  
        
    //a message 
    char *message = "ECHO Daemon v1.0 \r\n";  
    
    //initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }  
        
    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  
    
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
    
    //type of socket created 
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  
        
    //bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
        
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
        
    //accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Waiting for connections ...");  
        
    struct matrix * matrix;
    init_blur_matrix(&matrix);
    
    struct uint16_genome * firstGeneration[START_POPULATION], * nextGeneration[NEXT_GENERATION_POPULATION];
    for (int i = 0; i < START_POPULATION; i++) {
        firstGeneration[i] = (struct uint16_genome *)malloc(sizeof(struct uint16_genome));
        initRandGenome_uint16(firstGeneration[i]);
    }                                                  
    for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
        nextGeneration[i] = (struct uint16_genome *)malloc(sizeof(struct uint16_genome));
    }
    ssize_t rec;
    int count = 0, res;
    int creature_data[128];
    for (int i = 0; i < 128; i++) {
        creature_data[i] = RangedRandomNumber(0, 255);
    }


    while(TRUE)  
    {  
        //clear the socket set 
        FD_ZERO(&readfds);  
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
        //add child sockets to set 
        for ( i = 0 ; i < max_clients ; i++)  
        {  
            //socket descriptor 
            sd = client_socket[i];  
                
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
            
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds))  
        {  
            if ((new_socket = accept(master_socket, 
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  
            
            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
                  (address.sin_port));  
            
            for (int i = 0; i < 128; i++) {
                strcpy(buffer, to_string(creature_data[i]).c_str());
                cout << creature_data[i] << ' ';
                rec = 0;
                do {                        
                    int res = send(new_socket, &buffer[rec], bufsize-rec, 0);
                    rec+=res;
                } while (rec<bufsize);   
            }
            cout << '\n';
            //add new socket to array of sockets 
            for (i = 0; i < max_clients; i++)  
            {  
                //if position is empty 
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);  
                    FD_SET( client_socket[i] , &readfds);                 
                    
                    break;  
                }  
            }  
        }  

        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)  
        {  
            sd = client_socket[i];  
                
            if (FD_ISSET( sd , &readfds))  
            {  
                //incoming message
                 
                rec = 0;
                do {
                    int res = recv(client_socket[i], &buffer[rec], bufsize-rec, 0);
                    rec+=res;
                } while (rec<bufsize);
                
            }  
        }  
        int cur_clients = 0;
        for (i = 0; i < max_clients; i++) {
            if (client_socket[i] > 0) {
                cur_clients++;
            }
        }
        for (int i = 0; i < START_POPULATION; i++) {
            if (nextGeneration[i]->genes) {
                free(nextGeneration[i]->genes);
            }
            nextGeneration[i] = (struct uint16_genome *)malloc(sizeof(struct uint16_genome));
            copyGenome_uint16(firstGeneration[i], nextGeneration[i]);
        }
        for (int i = START_POPULATION; i < NEXT_GENERATION_POPULATION; i++) {
            if (nextGeneration[i]->genes) {
                free(nextGeneration[i]->genes);
            }
            crossGenome_uint16(firstGeneration[RangedRandomNumber(0, START_POPULATION - 1)], 
                                firstGeneration[RangedRandomNumber(0, START_POPULATION - 1)],
                                nextGeneration[i]);
        }


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


        int pos = 0;
        for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
            strcpy(buffer, to_string(nextGeneration[i]->size).c_str());
            rec = 0;
            do {
                int res = send(client_socket[pos], &buffer[rec], bufsize-rec, 0);
                rec+=res;
            } while (rec<bufsize);
            strcpy(buffer, to_string(i).c_str());
            rec = 0;
            do {
                int res = send(client_socket[pos], &buffer[rec], bufsize-rec, 0);
                rec+=res;
            } while (rec<bufsize);

            for (int j = 0; j < nextGeneration[i]->size; j++) {
                strcpy(buffer, to_string(nextGeneration[i]->genes[j]).c_str());
                rec = 0;
                do {
                    int res = send(client_socket[pos], &buffer[rec], bufsize-rec, 0);
                    rec+=res;
                } while (rec<bufsize);
            }


            pos++;
            if (pos == cur_clients || i == NEXT_GENERATION_POPULATION - 1) {
                for (int j = 0; j < pos; j++) {
                    rec = 0;
                    do {
                        int res = recv(client_socket[j], &buffer[rec], bufsize-rec, 0);
                        rec+=res;
                    } while (rec<bufsize);
                    if (atoi(buffer) == 123456789) {
                        rec = 0;
                        do {
                            int res = recv(client_socket[j], &buffer[rec], bufsize-rec, 0);
                            rec+=res;
                        } while (rec<bufsize);       
                    }
                    int genomeNumber = atoi(buffer);

                    rec = 0;
                    do {
                        int res = recv(client_socket[j], &buffer[rec], bufsize-rec, 0);
                        rec+=res;
                    } while (rec<bufsize);
                    nextGeneration[genomeNumber]->similarity = atof(buffer);                    
                }
                pos = 0;
            }
        }
        cout << '\n';


        for (int i = 0; i < START_POPULATION; i++) {
            int mostSimilar = 0; 
            int val = 1001;
            for (int j = 0; j < NEXT_GENERATION_POPULATION; j++) {
                if (nextGeneration[j]->similarity < val) {
                    val = nextGeneration[j]->similarity;
                    mostSimilar = j;
                }
            }
            free(firstGeneration[i]->genes);
            firstGeneration[i]->size = nextGeneration[mostSimilar]->size;
            firstGeneration[i]->genes = (uint16_t *)malloc(firstGeneration[i]->size * sizeof(uint16_t));
            for (int j = 0; j < firstGeneration[i]->size; j++) {
                firstGeneration[i]->genes[j] = nextGeneration[mostSimilar]->genes[j];
            }
            cout << "genome # " << mostSimilar << " is most similar " << nextGeneration[mostSimilar]->similarity << '\n';
            nextGeneration[mostSimilar]->similarity = 1001;
            
        }


        for (int i = 0; i < max_clients; i++) {

            if (client_socket[i] > 0) {
                strcpy(buffer, to_string(123456789).c_str());
                rec = 0;
                do {
                    int res = send(client_socket[i], &buffer[rec], bufsize-rec, 0);
                    rec+=res;
                } while (rec<bufsize);            }
        }

        struct genome * imageGenome = (struct genome * )malloc(sizeof(struct genome));
        uint16genome_to_genome(firstGeneration[0], imageGenome);
        struct creature * imageCreature = (struct creature *)malloc(sizeof(struct creature));
        initCreature(imageCreature);
        for (int i = 0; i < 128; i++) { 
            for (int j = 0; j < N * N; j++) {
                imageCreature->cells[j].v[i] = imageCreature->cells[j].dv[i] = creature_data[i];
            }
        }
        int step = 0;
        while(imageCreature->n < MAX_CREATURE_SIZE) {
            calculateDvForCells_v2(imageCreature, imageGenome);
            applyDvForCells(imageCreature);
            diff_v2(imageCreature, matrix);
            applyDiff(imageCreature);
            if (step % 2 == 0) {
                imageCreature = grow(imageCreature);
            }
            step++;
        }

        char filename[FILENAME_MAX];
        sprintf(filename, "c%d.bmp", count);
        createImage(imageCreature, filename);
    

        cout << "//////////////////////////\n      done generation # " << count<< "//////////////////////////\n";
        count++;

    }  
    
    
    for (int i = 0; i < NEXT_GENERATION_POPULATION; i++) {
        if (nextGeneration[i]) {
            free(nextGeneration[i]->genes);
            free(nextGeneration[i]);
        }
    }

    for (int i = 0; i < START_POPULATION; i++) {
        if (firstGeneration[i]) {
            free(firstGeneration[i]->genes);
            free(firstGeneration[i]);
        }
    }

    return 0;  
}  
