#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

using namespace std;

struct dna {
    string genes;
    float fitness;
};

int main() {
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
    recv(client, buffer, bufsize, 0);
    int NUMBER_OF_SURVIVORS = atoi(buffer);
    recv(client, buffer, bufsize, 0);
    int NUMBER_OF_GENOMES = atoi(buffer);
    recv(client, buffer, bufsize, 0);
    int NUMBER_OF_CREATURES = atoi(buffer);

    int i,j, length, count, mostFit;
    float maxFit;
    dna generation[NUMBER_OF_CREATURES];
// loop to send messages to server
    do {
        for (i = 0; i < NUMBER_OF_CREATURES; i++) {
            ssize_t rec = 0;
            do {
                int res = recv(client, &buffer[rec], sizeof(buffer)-rec, 0);
                rec+=res;
            } while (rec<sizeof(buffer));
            generation[i].genes = buffer;
            length = generation[i].genes.length();
            count = 0;
            for (j = 0; j < length * 2 / NUMBER_OF_GENOMES; j++) {

                for (int k = NUMBER_OF_GENOMES/2 - 1; k >= 0; k--) {
                    int a = 1;
                    for (int l = 1; l < NUMBER_OF_GENOMES/2-k; l++) {
                            a*=2;
                    }
                    if (generation[i].genes[j*NUMBER_OF_GENOMES/2 + k] == '1') {
                        count +=a;
                    }
                }
            }
            generation[i].fitness = (float)count * (float)NUMBER_OF_GENOMES / (float)length;
        }
        for (i = 0; i < NUMBER_OF_CREATURES; i++) {
            cout << generation[i].genes << endl;
            cout<<generation[i].fitness<<"-------------------------------"<< endl;
        }
        for (i = 0; i < NUMBER_OF_SURVIVORS; i++) {
            maxFit = -1.0;
            mostFit = -1;
            for (j = 0; j < NUMBER_OF_CREATURES; j++) {
                if (generation[j].fitness > maxFit) {
                    maxFit = generation[j].fitness;
                    mostFit = j;
                }
            }
            strcpy(buffer, generation[mostFit].genes.c_str());
            ssize_t rec = 0;
                do {
                    int res = send(client, &buffer[rec], bufsize-rec, 0);
                    rec+=res;
                } while (rec<bufsize);
            generation[mostFit].fitness = -1.0;    
        }
    } while (!isExit);

 /* ---------------- CLOSE CALL ------------- */
    cout << "\nConnection terminated.\n";
    close(client);
    return 0;
}
