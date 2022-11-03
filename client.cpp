// This program is intended to be run on the client side. It will initiate connection with a server and push a file using segmentation

#include "unp.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

#define SEGMENT_SIZE 512

void empty_buffer(char buffer[], int size);

int gremlins(char buffer[], double corruptionChance, double lossChance);

int main(int argc, char **argv) {
    int sd;
    struct sockaddr_in server;
    socklen_t serAddrLen;

    std::string input_name;

    sd = socket(AF_INET,SOCK_DGRAM,0);

    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    while(true){
        std::cout << "File name: " << std::flush;
        std::getline(std::cin, input_name);

        char * input_name_cstr = new char [input_name.length()+1];
        std::strcpy(input_name_cstr, input_name.c_str()); //convert from string to cstring

        char packet[input_name.length()+4] = {}; //create the packet that will be sent. +4 because string length, +1 for null, +3 for GET
        
        //populate "packet" with GET and the file name
        packet[0] = 'G';
        packet[1] = 'E';
        packet[2] = 'T';
        std::copy(input_name_cstr, input_name_cstr+input_name.length()+1, packet + 3);
        
        sendto(sd, packet, 1, 0, (struct sockaddr*)&server, sizeof(server));

        //if file exists then break (needs implemented)
        break;
    }


    int n;
    char message_buffer[512];
    serAddrLen = sizeof(server);
    std::ofstream file;

    file.open("received.bin");

    char file_data_buffer[504] = {};

    for (;;) {
        std::cout << "Waiting for packet..." << std::endl;
        
        n = recvfrom(sd, message_buffer, 512, 0, (struct sockaddr*)&server, &serAddrLen);
        std::cout << "Got " << n << " bytes in response" << std::endl;

        std::memcpy(file_data_buffer, &message_buffer[8], 504);
        file_data_buffer[504] = '\0';

        size_t len = strlen(file_data_buffer); // will calculate number of non-0 symbols before first 0
        char * newBuf = (char *)malloc(len); // allocate memory for new array, don't forget to free it later
        memcpy(newBuf, file_data_buffer, len);

        if (message_buffer[0] != '\0') {
            file.write(newBuf, len);
        } else {
            std::cout << "End of transmission" << std::endl;
            file.close();
        }

        empty_buffer(file_data_buffer, 504);
        empty_buffer(message_buffer, 512);
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
    srand(time(NULL));

    //Error Checking.
    if (corruptionChance > 1 || corruptionChance < 0 || lossChance > 1 || lossChance < 0) { 
        return -1;
    } 


    if(rand()/RAND_MAX < lossChance){ //Checks for loss of packet
        return 1;
    }
    else if (rand()/RAND_MAX < corruptionChance) { //Checks for corruption of packet
        randomNum = rand()/RAND_MAX;
        if(randomNum <= 0.7){ //70% only one packet is affected
            randomByte = rand() % 512;
            buffer[randomByte] = '\0';
        }
        
        if(randomNum <= 0.2){ //20% chance two packets are affected
            randomByte = rand() % 512;
            buffer[randomByte] = '\0';
        }

        if(randomNum <= 0.1){ //10% chance three packets are affected
            randomByte = rand() % 512;
            buffer[randomByte] = '\0';
        }
        return 2;
    }

    return 0;
 
}