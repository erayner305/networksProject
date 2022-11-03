#include "unp.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int SEGMENT_SIZE = 512;
int CHECKSUM_SIZE = 4;
int PACKET_COUNT_SIZE = 4;
int INSTRUCTION_SIZE = 3;
int HEADER_SIZE = CHECKSUM_SIZE + PACKET_COUNT_SIZE;
int DATA_SIZE = SEGMENT_SIZE - HEADER_SIZE;

void empty_buffer(char buffer[], int size);

void generate_checksum(char input_buffer[], char output_buffer[]);

void generate_packet_num(uint32_t packet_num, char packet_num_buffer[]);

int main() {

    // Start our network connection code

    int n, sd, client_fd;
    struct sockaddr_in server;
    socklen_t serverLen = sizeof(server);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(SERV_PORT);

    sd = socket(AF_INET, SOCK_DGRAM, 0);

    // Bind the server to the socket
    bind(sd, (struct sockaddr *)&server, sizeof(server));

    listen(sd, 10);

    struct sockaddr_storage client_addr;
    socklen_t client_size = sizeof(client_addr);
    client_fd = accept(sd, (struct sockaddr *)&client_addr, &client_size);


    std::cout << "Ready" << std::endl;


    // Integer to keep track of our packet counts
    int packet_count = 0;
    
    // Define char buffers  based on our segment size
    char data_buffer[DATA_SIZE] = {};
    
    // Define an empty buffer for our generated checksum
    char checksum_buffer[CHECKSUM_SIZE] = {};

    // Define an empty buffer for our generated checksum
    char packet_count_buffer[PACKET_COUNT_SIZE] = {};   

    // Define our empty packet
    char packet[SEGMENT_SIZE] = {};

    // Define our buffer to store our incoming message
    char message_buffer[SEGMENT_SIZE] = {};

    // Define a buffer to store the GET instruction of the message 
    char instruction_buffer[3] = {};

    // Define a buffer to store the filename passed in the GET packet
    char filename_buffer[SEGMENT_SIZE - INSTRUCTION_SIZE] = {};

    // Input string to open
    std::string input_name;

    // File in stream
    std::ifstream file_in;


    while (true) {
        n = recvfrom(sd, message_buffer, sizeof(message_buffer), 0, (struct sockaddr *)&server, &serverLen);

        std::copy(message_buffer, message_buffer+4, instruction_buffer);

        if (message_buffer == std::string("GET").c_str()) {

            std::copy(message_buffer+4, message_buffer+SEGMENT_SIZE, filename_buffer);

            std::string target_filename = std::string(filename_buffer);

            file_in.open(target_filename.c_str(), std::ios_base::binary);

            if (file_in) {

                // File requested exists, send all of the packets for the file
                while(!file_in.eof()){

                    file_in.read(data_buffer, DATA_SIZE);

                    // Generate our checksum using our buffer
                    generate_checksum(data_buffer, checksum_buffer);
                    generate_packet_num(++packet_count, packet_count_buffer);

                    std::copy(checksum_buffer, checksum_buffer+CHECKSUM_SIZE, packet);
                    std::copy(packet_count_buffer, packet_count_buffer+PACKET_COUNT_SIZE, packet+CHECKSUM_SIZE);
                    std::copy(data_buffer, data_buffer+DATA_SIZE, packet+8);

                    // Send packet to client
                    sendto(sd, packet, SEGMENT_SIZE, 0, (struct sockaddr *)&server, sizeof(server));

                    empty_buffer(packet, SEGMENT_SIZE);
                    empty_buffer(data_buffer, DATA_SIZE);

                    // Sleep in the event of packet overflow
                    usleep(100);
                }

                sendto(sd, "\0", 1, 0, (struct sockaddr *)&server, sizeof(server));

            } else {

                std::cout << "[Error] Recieved request for file " << target_filename << " that does not exist" << std::endl;
                sendto(sd, "ERR", 1, 0, (struct sockaddr*)&server, sizeof(server));

            }

            empty_buffer(packet, SEGMENT_SIZE);
            empty_buffer(message_buffer, SEGMENT_SIZE);
            empty_buffer(data_buffer, DATA_SIZE);
            
            file_in.close();
            packet_count = 0;
        }
    }
    
    close(sd);

    return 0;
}


void empty_buffer(char buffer[], int size) {
    for (int i = 0; i < size; i++) {
        buffer[i] = '\0';
    }
}


void generate_packet_num(uint32_t packet_num, char packet_num_buffer[]) {
    std::cout << "Packet Number: " << packet_num << std::endl;
    memcpy(packet_num_buffer, &packet_num, sizeof(packet_num));
}


void generate_checksum(char data_buffer[], char checksum_buffer[]) {
    uint32_t sum = 0;
    for(int i = 0; i < DATA_SIZE; i++) {
        sum += data_buffer[i];
    }
    std::cout << "Checksum: " << sum << std::endl;
    memcpy(checksum_buffer, &sum, sizeof(sum));
}