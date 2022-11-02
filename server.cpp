// This program is intended to be run on the client side. It will initiate connection with a server and push a file using segmentation

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int SEGMENT_SIZE = 512;
int CHECKSUM_SIZE = 4;
int DATA_SIZE = SEGMENT_SIZE - CHECKSUM_SIZE;

void empty_buffer(char buffer[]);

void generate_checksum(char input_buffer[], char output_buffer[]);


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

    // Define our empty packet
    char packet[SEGMENT_SIZE] = {};

    while(!file_in.eof()){
        file_in.read(data_buffer, DATA_SIZE);

        // Generate our checksum using our buffer
        generate_checksum(data_buffer, checksum_buffer);

        std::copy(checksum_buffer, checksum_buffer + CHECKSUM_SIZE, packet);
        std::copy(data_buffer, data_buffer + DATA_SIZE, packet + CHECKSUM_SIZE);

        // Implementation of packet here....
        for (int i = 0; i < SEGMENT_SIZE; i++) {
            std::cout << (uint)packet[i] << std::endl;
        }

        empty_buffer(packet);

        std::cout << std::endl << "End of Packet " << ++packet_count << std::endl;
    }

    file_in.close();

    return 0;
}


void empty_buffer(char buffer[]) {
    for (int i = 0; i < SEGMENT_SIZE; i++) {
        buffer[i] = '\0';
    }
}


void generate_packet_num(int packet_num, char packet_num_buffer[]) {
    int shift_value = 16;
    for (int i = 0; i < 8; i++) { 
        packet_num_buffer[i] = (packet_num >> (shift_value -= 4)) & 0xFF; 
    }
}


void generate_checksum(char data_buffer[], char checksum_buffer[]) {
    uint32_t sum = 0;
    for(int i = 0; i < DATA_SIZE; i++) {
        sum += data_buffer[i];
    }
    std::cout << sum << std::endl;
    int shift_value = 16;
    for (int i = 0; i < 4; i++) { 
        checksum_buffer[i] = (sum >> (shift_value -= 4)) & 0xFF;
        std::cout << "shifted: " << ((sum >> (shift_value -= 4)) & 0xFF) << std::endl;
        std::cout << "array: " << (uint)checksum_buffer[i] << std::endl;
    }
}