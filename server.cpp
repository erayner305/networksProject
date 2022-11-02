// This program is intended to be run on the client side. It will initiate connection with a server and push a file using segmentation

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdio.h>

int SEGMENT_SIZE = 512;
int CHECKSUM_SIZE = 4;
int PACKET_COUNT_SIZE = 4;
int HEADER_SIZE = CHECKSUM_SIZE + PACKET_COUNT_SIZE;
int DATA_SIZE = SEGMENT_SIZE - HEADER_SIZE;

void empty_buffer(char buffer[]);

void generate_checksum(char input_buffer[], char output_buffer[]);

void generate_packet_num(uint32_t packet_num, char packet_num_buffer[]);

int main() {

    // Input string to open
    std::string input_name;

    // File in stream
    std::ifstream file_in;

    while (true)
    {
        std::cout << "File name: " << std::flush;
        std::getline(std::cin, input_name);
        file_in.open(input_name.c_str(), std::ios_base::binary);
        if (file_in) break;
        std::cout << "Invalid file. Please enter a valid file name " << std::endl << std::flush;
    }

    // Integer to keep track of our packet counts
    int packet_count = 0;
    
    // Define an empty buffer based on our segment size
    char data_buffer[DATA_SIZE] = {};
    
    // Define an empty buffer for our generated checksum
    char checksum_buffer[CHECKSUM_SIZE] = {};

    // Define an empty buffer for our generated checksum
    char packet_count_buffer[PACKET_COUNT_SIZE] = {};

    // Define our empty packet
    char packet[SEGMENT_SIZE] = {};


    while(!file_in.eof()){
        file_in.read(data_buffer, DATA_SIZE);

        // Generate our checksum using our buffer
        generate_checksum(data_buffer, checksum_buffer);
        generate_packet_num(++packet_count, packet_count_buffer);

        std::copy(checksum_buffer, checksum_buffer+CHECKSUM_SIZE, packet);
        std::copy(packet_count_buffer, packet_count_buffer+PACKET_COUNT_SIZE, packet+CHECKSUM_SIZE);
        std::copy(data_buffer, data_buffer+DATA_SIZE, packet+8);

        // Implementation of packet here....
        for (int i = 0; i < SEGMENT_SIZE; i++) {
            std::cout << (uint8_t)packet[i] << std::endl;
        }

        empty_buffer(packet);
    }

    file_in.close();

    return 0;
}


void empty_buffer(char buffer[]) {
    for (int i = 0; i < SEGMENT_SIZE; i++) {
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