// This program is intended to be run on the client side. It will initiate connection with a server and push a file using segmentation

#include "unp.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

int SEGMENT_SIZE = 512;
double CORRUPTION_CHANCE = 0.1;
double LOSS_CHANCE = 0.1;

void empty_buffer(char buffer[], int size);

int gremlins(char buffer[], double corruptionChance, double lossChance);

int main(int argc, char **argv) {
    int sd;
    struct sockaddr_in server;
    socklen_t serAddrLen;

    sd = socket(AF_INET,SOCK_DGRAM,0);

    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    sendto(sd, "G", 1, 0, (struct sockaddr*)&server, sizeof(server));

    int n;
    char message_buffer[512];
    serAddrLen = sizeof(server);

    std::ofstream file;
    file.open("received.bin");

    char file_data_buffer[504] = {};

    for (;;) {
        n = recvfrom(sd, message_buffer, 512, 0, (struct sockaddr*)&server, &serAddrLen);

        std::cout << "[Info] Got " << n << " bytes in response" << std::endl;

        int packet_status = gremlins(message_buffer, CORRUPTION_CHANCE, LOSS_CHANCE);

        if(packet_status != 1) {
            std::memcpy(file_data_buffer, &message_buffer[8], 504);
            file_data_buffer[504] = '\0';

            size_t len = strlen(file_data_buffer); // will calculate number of non-0 symbols before first 0
            char * newBuf = (char *)malloc(len); // allocate memory for new array, don't forget to free it later
            memcpy(newBuf, file_data_buffer, len);

            if (message_buffer[0] != '\0') {
                file.write(newBuf, len);
            } else {
                std::cout << "[Info] End of transmission" << std::endl;
                file.close();
            }

            empty_buffer(file_data_buffer, 504);
            empty_buffer(message_buffer, 512);
        }        
    }
    
    return 0;
}


void empty_buffer(char buffer[], int size) {
    for (int i = 0; i < size; i++) {
        buffer[i] = '\0';
    }
}


int gremlins(char buffer[], double corruptionChance, double lossChance){
    double randomNum;
    int randomByte;
    srand(rand()*time(NULL));

    //Error Checking.
    if (corruptionChance > 1 || corruptionChance < 0 || lossChance > 1 || lossChance < 0) { 
        return -1;
    } 

    double rand_losschance = (double) rand() / RAND_MAX;

    if(rand_losschance < lossChance){ //Checks for loss of packet
        std::cout << "[Gremlin] Packet was lost" << std::endl;
        return 1;
    }
    else if ((double) rand()/RAND_MAX < corruptionChance) { //Checks for corruption of packet
        randomNum = rand()/RAND_MAX;
        if(randomNum <= 0.7){ //70% only one packet is affected
            //std::cout << "[Gremlin] 1/3 bytes were affected" << std::endl;
            randomByte = rand() % 512;
            buffer[randomByte] = '1';
        }
        
        if(randomNum <= 0.2){ //20% chance two packets are affected
            //std::cout << "[Gremlin] 2/3 bytes were affected" << std::endl;
            randomByte = rand() % 512;
            buffer[randomByte] = '1';
        }

        if(randomNum <= 0.1){ //10% chance three packets are affected
            //std::cout << "[Gremlin] 3/3 bytes were affected" << std::endl;
            randomByte = rand() % 512;
            buffer[randomByte] = '1';
        }
        return 2;
    }

    return 0;
 
}