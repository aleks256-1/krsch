#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <random>
#include <functional>
#include <math.h>
#include <chrono>
#include <thread>
#include <time.h>
#include <string>

using namespace std;

#define NUMBER_OF_SURVIVORS 1000
#define NUMBER_OF_CREATURES 10000
#define NUMBER_OF_GENOMES 16
#define MUTATED_POPULATION 0.5


class backoff_time_t {
public:
  random_device                      rd;
  mt19937                            mt;
  uniform_real_distribution<double>  dist;
  backoff_time_t() : rd{}, mt{rd()}, dist{0.0, 1.0} {}

  double rand() {
    return dist(mt);
  }
};

thread_local backoff_time_t backoff_time;





int RangedRandomNumber(int min, int max) {
    std::random_device rd;
    std::mt19937 zGenerator(rd());
    std::uniform_int_distribution<int> uni(min, max);
    return uni(zGenerator); 
} 

struct dna {
    string genes;
    float fitness;
    bool isUnique;
};

string GenerateRandomDna(int maxLen) {
    string newDna;
    for (int i = 0; i < maxLen; i++) {
        if (backoff_time.rand() < 0.5) {
            newDna += '0';
        } else {
            newDna += '1';
        }
    }
    return newDna;
}

string CrossParents(string a, string b){
    vector<vector<int> > matrixOfEntries;
    int aSize = a.size()/NUMBER_OF_GENOMES;
    int bSize = b.size()/NUMBER_OF_GENOMES;
    matrixOfEntries.resize(aSize + 1);
    for(int i = 0; i <= static_cast<int>(aSize); i++)
        matrixOfEntries[i].resize(bSize + 1);
    for(int i = static_cast<int>(aSize) - 1; i >= 0; i--) {
        for(int j = static_cast<int>(bSize) - 1; j >= 0; j--) {
            if(a.substr(i*NUMBER_OF_GENOMES, NUMBER_OF_GENOMES) == b.substr(j*NUMBER_OF_GENOMES,NUMBER_OF_GENOMES)) {
                matrixOfEntries[i][j] = 1 + matrixOfEntries[i+1][j+1];
            } else {
                matrixOfEntries[i][j] = max(matrixOfEntries[i+1][j], matrixOfEntries[i][j+1]);                
            }
        }
    }
    string res, fragment1 = "", fragment2 = "";
    for(int i = 0, j = 0; i < static_cast<int>(aSize) || j < static_cast<int>(bSize); ) {
//        cout<<"f1 "<<fragment1<<endl<<"f2 "<<fragment2<<endl<<"res "<<res<<endl;
        if(i < static_cast<int>(aSize) && j < static_cast<int>(bSize) && a.substr(i*NUMBER_OF_GENOMES, NUMBER_OF_GENOMES) == b.substr(j*NUMBER_OF_GENOMES, NUMBER_OF_GENOMES)) {
            if (fragment2.length()==0) {
                res+=fragment1;
            } else if (fragment1.length() == 0) {
                res+=fragment2;
            } else  if (backoff_time.rand() < 0.5) {
                res+=fragment1;
            } else {
                res+=fragment2;
            }
            fragment1="";
            fragment2="";
            res+=(a.substr(i*NUMBER_OF_GENOMES,NUMBER_OF_GENOMES));
            i++;
            j++;
        } else {
            if(i < static_cast<int>(aSize) && matrixOfEntries[i][j] == matrixOfEntries[i+1][j]) {
                fragment1+=a.substr(i*NUMBER_OF_GENOMES,NUMBER_OF_GENOMES);
                i++;
            } else if (j < static_cast<int>(bSize)) {
                fragment2+=b.substr(j*NUMBER_OF_GENOMES,NUMBER_OF_GENOMES);
                j++;
            }          
        }
    }
    if (fragment2.length()==0) {
        res+=fragment1;
    } else if (fragment1.length() == 0) {
        res+=fragment2;
    } else  if (backoff_time.rand() < 0.5) {
        res+=fragment1;
    } else {
         res+=fragment2;
    }
    return res;
}

string ChangeRandomBytes(const string creature) {
    string res = creature;
    int len = res.length();
    for (int i = 0; i < len; i++) {
        if (backoff_time.rand() > 0.25) {
            if (res[i] == '0') {
                res[i] = '1';
            } else {
                res[i] = '0';
            }
        }
    }
    return res;
}

string RandomFragmentInsetrion(const string creature) {
    string res = creature, randomFragment;
    int len = res.length();
    randomFragment = GenerateRandomDna((int)(backoff_time.rand()*10)*NUMBER_OF_GENOMES);
    return res.insert((int)(backoff_time.rand()*len), randomFragment);
}

int RandomCreature() {
    return (int)(backoff_time.rand()*NUMBER_OF_CREATURES);
}

string RandomFragmentDeletion(const string creature) {
    string res;
    int length = creature.length() / NUMBER_OF_GENOMES;
    int startPosition = RangedRandomNumber(0, length-1);
    int fragmentsToRemove;
    if ((length - startPosition)/2 > 0) {
        fragmentsToRemove = RangedRandomNumber(1, (length - startPosition)/2);
    } else {
        fragmentsToRemove = 0;
    }
    if (startPosition + fragmentsToRemove == length && startPosition == 0) {
        fragmentsToRemove--;
    }
    res = creature.substr(0, startPosition * NUMBER_OF_GENOMES) 
        +creature.substr((startPosition + fragmentsToRemove) * NUMBER_OF_GENOMES, (length - fragmentsToRemove - startPosition) * NUMBER_OF_GENOMES);
    return res;
}

string RandomFragmentDuplicate(const string creature) {
    string res;
    int length = creature.length() / NUMBER_OF_GENOMES;
    int startPosition = RangedRandomNumber(0, length - 1);
    int fragmentsToDuplicate = RangedRandomNumber(1, (length - startPosition));
    res = creature.substr(0, (startPosition+fragmentsToDuplicate)*NUMBER_OF_GENOMES) 
        + creature.substr(startPosition * NUMBER_OF_GENOMES, (length - startPosition) * NUMBER_OF_GENOMES);
    return res;
}

string RandomFragmentMove(const string creature) {
    string res;
    int length = creature.length() / NUMBER_OF_GENOMES;
    int startPosition = RangedRandomNumber(0,length - 1);
    int fragmentsToMove = RangedRandomNumber(1, (length - startPosition));
    if (startPosition + fragmentsToMove == length && startPosition == 0) {
        fragmentsToMove--;
    }
    cout << startPosition<<" "<<fragmentsToMove<<endl;
    res = creature.substr(0, startPosition * NUMBER_OF_GENOMES) 
        + creature.substr((startPosition + fragmentsToMove) * NUMBER_OF_GENOMES, (length - startPosition - fragmentsToMove) * NUMBER_OF_GENOMES);
    int newPosition = RangedRandomNumber(0, res.length() / NUMBER_OF_GENOMES - 1);
    cout << res<< endl<<newPosition<<endl;
    res.insert(newPosition, creature.substr(startPosition * NUMBER_OF_GENOMES, fragmentsToMove * NUMBER_OF_GENOMES));
    return res;
}

string RandomFragmentCopy(const string creature) {
    string res = creature;
    int length = creature.length() / NUMBER_OF_GENOMES;
    int startPosition = RangedRandomNumber(0,length - 1);
    int fragmentsToCopy = RangedRandomNumber(1, (length - startPosition));
    int newPosition = RangedRandomNumber(0,length - 1);
    res.insert(newPosition * NUMBER_OF_GENOMES, creature.substr(startPosition * NUMBER_OF_GENOMES, fragmentsToCopy * NUMBER_OF_GENOMES));
    return res;
}

struct individualsToExchangeDnaFragments {
    string first;
    string second;
};

individualsToExchangeDnaFragments ExchangeDnaFragments (string firstCreature, string secondCreature) {
    individualsToExchangeDnaFragments res;
    int lenFirst = firstCreature.length() / NUMBER_OF_GENOMES;
    int lenSecond = secondCreature.length() / NUMBER_OF_GENOMES;
    int firstFragmentStart = RangedRandomNumber(0, lenFirst - 1);
    int secondFragmentStart = RangedRandomNumber(0, lenSecond - 1);
    int firstNumOfFragments = RangedRandomNumber(1, lenFirst - firstFragmentStart);
    int secondNumOfFragments = RangedRandomNumber(1, lenSecond - secondFragmentStart);
    if (firstFragmentStart + firstNumOfFragments == lenFirst && firstFragmentStart == 0 && firstNumOfFragments > 1) {
        firstNumOfFragments--;
    }
    if (secondFragmentStart + secondNumOfFragments == lenSecond && secondFragmentStart == 0 && secondNumOfFragments > 1) {
        secondNumOfFragments--;
    }
    res.first = firstCreature.substr(0,firstFragmentStart * NUMBER_OF_GENOMES) 
        + secondCreature.substr(secondFragmentStart * NUMBER_OF_GENOMES, secondNumOfFragments * NUMBER_OF_GENOMES)
        + firstCreature.substr((firstFragmentStart + firstNumOfFragments) * NUMBER_OF_GENOMES, (lenFirst - firstFragmentStart - firstNumOfFragments) * NUMBER_OF_GENOMES);
    res.second = secondCreature.substr(0,secondFragmentStart * NUMBER_OF_GENOMES) 
        + firstCreature.substr(firstFragmentStart * NUMBER_OF_GENOMES, firstNumOfFragments * NUMBER_OF_GENOMES)
        + secondCreature.substr((secondFragmentStart + secondNumOfFragments) * NUMBER_OF_GENOMES, (lenSecond - secondFragmentStart - secondNumOfFragments) * NUMBER_OF_GENOMES);
    return res;
}


individualsToExchangeDnaFragments Mutate(const string creature, const string secondCreature) {
    individualsToExchangeDnaFragments ret;
    ret.first = creature;
    ret.second = secondCreature;
    for (int i = 0; i < 5; i++) {
        int mutation = (int)(backoff_time.rand());
        if (mutation < 0.14) {
            ret.first = ChangeRandomBytes(creature);
        } else if (mutation < 0.28) {
            ret.first = RandomFragmentDeletion(creature);
        } else if (mutation < 0.42) {
            ret.first = RandomFragmentInsetrion(creature);
        } else if (mutation < 0.56) {                                      
            ret.first = RandomFragmentDuplicate(creature);
        } else if (mutation < 0.70) {                                      
            ret.first = RandomFragmentMove(creature);
        } else if (mutation < 0.84) {                                      
            ret.first = RandomFragmentCopy(creature);
        } else if (mutation < 1.00) {                                                
            ret = ExchangeDnaFragments(creature, secondCreature);
        }
    }
    return ret;    
}


int main() {
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
//отправляем клиенту данные о модели
    dna firstGeneration[NUMBER_OF_SURVIVORS], nextGeneration[NUMBER_OF_CREATURES];
    strcpy(buffer, to_string(NUMBER_OF_SURVIVORS).c_str());
    send(client, buffer, bufsize, 0);
    strcpy(buffer, to_string(NUMBER_OF_GENOMES).c_str());
    send(client, buffer, bufsize, 0);
    strcpy(buffer, to_string(NUMBER_OF_CREATURES).c_str());
    send(client, buffer, bufsize, 0);
    int i,j, count = 0;
//Строим первое поколение
    for (i=0; i<NUMBER_OF_SURVIVORS; i++) {
        firstGeneration[i].genes = GenerateRandomDna(RangedRandomNumber(100,200) * NUMBER_OF_GENOMES);
        firstGeneration[i].fitness = 0.0;
    }
    while (client > 0) {
 // loop to recive messages from client
        do {
//Строим следующее поколение
            for (i = 0; i < NUMBER_OF_SURVIVORS; i++) {
                nextGeneration[i].genes = firstGeneration[i].genes;
                nextGeneration[i].fitness = 0.0;
            }
            for (i = NUMBER_OF_SURVIVORS; i < NUMBER_OF_CREATURES; i++) {
                nextGeneration[i].genes = CrossParents(firstGeneration[(int)(backoff_time.rand()*NUMBER_OF_SURVIVORS)].genes, firstGeneration[(int)(backoff_time.rand()*NUMBER_OF_SURVIVORS)].genes);
                nextGeneration[i].fitness = 0.0;
            }
//100% мутируем повторяющиеся существа
            for (i = 0; i < NUMBER_OF_CREATURES; i++) {
                for (j = i; j < NUMBER_OF_CREATURES; j++) {
                    if (nextGeneration[i].genes == nextGeneration[j].genes) {
                        individualsToExchangeDnaFragments individuals;
                        int secondCreature = RandomCreature();
                        individuals = Mutate(nextGeneration[j].genes, nextGeneration[secondCreature].genes);
                        nextGeneration[j].genes = individuals.first;
                        nextGeneration[secondCreature].genes = individuals.second;
                    }
                }
                
            }
//Мутируем
            int numberOfMutations = (int)(backoff_time.rand() * NUMBER_OF_CREATURES * MUTATED_POPULATION), creature, mutation;
            for (i = 0; i < numberOfMutations; i++) {
                mutation = (int)(backoff_time.rand());
                creature = RandomCreature();
                if (mutation < 0.14) {
                    nextGeneration[creature].genes = ChangeRandomBytes(nextGeneration[creature].genes);
                } else if (mutation < 0.28) {
                    nextGeneration[creature].genes = RandomFragmentDeletion(nextGeneration[creature].genes);
                } else if (mutation < 0.42) {
                    nextGeneration[creature].genes = RandomFragmentInsetrion(nextGeneration[creature].genes);
                } else if (mutation < 0.56) {                                      
                    nextGeneration[creature].genes = RandomFragmentDuplicate(nextGeneration[creature].genes);
                } else if (mutation < 0.70) {                                      
                    nextGeneration[creature].genes = RandomFragmentMove(nextGeneration[creature].genes);
                } else if (mutation < 0.84) {                                      
                    nextGeneration[creature].genes = RandomFragmentCopy(nextGeneration[creature].genes);
                } else if (mutation < 1.00) {                                      
                    int secondCreature = RandomCreature();               
                    individualsToExchangeDnaFragments individuals = ExchangeDnaFragments(nextGeneration[creature].genes, nextGeneration[secondCreature].genes);
                    nextGeneration[creature].genes = individuals.first;
                    nextGeneration[secondCreature].genes = individuals.second;
                }
            }
//Отправляем поколение клиенту
            for (i = 0; i < NUMBER_OF_CREATURES; i++) {
                if (nextGeneration[i].genes.length() == 0) {
                    cout<<"a NULL creature #"<<i<<endl;
                }
                strcpy(buffer, nextGeneration[i].genes.c_str());
                ssize_t rec = 0;
                do {
                    int res = send(client, &buffer[rec], bufsize-rec, 0);
                    rec+=res;
                } while (rec<bufsize);
            }
//Получаем выживших
            for (i = 0; i < NUMBER_OF_SURVIVORS; i++) {
                ssize_t rec = 0;
                do {
                    int res = recv(client, &buffer[rec], bufsize-rec, 0);
                    rec+=res;
                } while (rec<bufsize);
                firstGeneration[i].genes = buffer;
            }
            cout<<"done #"<<count<<endl;
            count++;
        } while (!isExit);

 /* ---------------- CLOSE CALL ------------- */
        cout << "\n\n=> Connection terminated with IP " << inet_ntoa(server_addr.sin_addr);
        close(client);
        cout << "\nGoodbye..." << endl;
        exit(1);
    }
 /* ---------------- CLOSE CALL ------------- */
    close(server);
    return 0;
}
