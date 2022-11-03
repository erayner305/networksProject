// This program is intended to be run on the client side. It will initiate connection with a server and push a file using segmentation

#include "unp.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

int SEGMENT_SIZE = 512;
char GET_INSTR[4] = "GET";
char ACK_INSTR[4] = "ACK";

void empty_buffer(char buffer[], int size);

int gremlins(char buffer[], double corruptionChance, double lossChance);

int main(int argc, char **argv) {
    int sd;
    struct sockaddr_in server;
    socklen_t serAddrLen;
    serAddrLen = sizeof(server);

    sd = socket(AF_INET,SOCK_DGRAM,0);

    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    int n;
    char message_buffer[512];
    std::string input_filename;
    std::ofstream downloaded_file;

    char file_data_buffer[504] = {};
    char packet_instruction[4] = {};

    while(true) {
        std::cout << "File name to download: " << std::flush;
        std::getline(std::cin, input_filename);

        char packet[512] = {};
        
        //populate "packet" with GET and the file name
        strcpy(packet, GET_INSTR);
        memcpy(packet+4, input_filename.c_str(), input_filename.length());
        
        sendto(sd, packet, SEGMENT_SIZE, 0, (struct sockaddr*)&server, sizeof(server));

        n = recvfrom(sd, message_buffer, 512, 0, (struct sockaddr*)&server, &serAddrLen);
        memcpy(packet_instruction, message_buffer, 4);
        
        std::cout << "Response: " << packet_instruction << std::endl;

        if (strcmp(packet_instruction, ACK_INSTR) == 0) {
            
            // File exists
            downloaded_file.open("recv_" + input_filename);

            empty_buffer(message_buffer, 512);

            for (;;) {
                n = recvfrom(sd, message_buffer, 512, 0, (struct sockaddr*)&server, &serAddrLen);
                std::cout << "[Info] Got " << n << " bytes in response" << std::endl;

                if (message_buffer[0] == '\0') {
                    break;
                }

                std::memcpy(file_data_buffer, &message_buffer[8], 504);
                file_data_buffer[504] = '\0';

                size_t len = strlen(file_data_buffer); // will calculate number of non-0 symbols before first 0
                char * newBuf = (char *)malloc(len); // allocate memory for new array, don't forget to free it later
                memcpy(newBuf, file_data_buffer, len);

                downloaded_file.write(newBuf, len);
            }
            
            std::cout << "[Info] End of transmission" << std::endl;

            downloaded_file.close();
            
        } else {
            std::cout << "[Error] File name does not exist on server, please try again" << std::endl;
        }

        empty_buffer(file_data_buffer, 504);
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