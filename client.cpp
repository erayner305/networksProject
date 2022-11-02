// This program is intended to be run on the client side. It will initiate connection with a server and push a file using segmentation

#include <iostream>
#include <fstream>
#include <vector>

#define SEGMENT_SIZE 512

void empty_buffer(char buffer[]);

int main() {
    // Input string to open
    std::string input_name;

    // Define an empty buffer based on our segment size
    char buffer[SEGMENT_SIZE] = {};

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


    int packetCount = 0;

    while(!file_in.eof()){
        file_in.read(buffer, SEGMENT_SIZE);

        // Implementation of packet here....
        for (int i = 0; i < SEGMENT_SIZE; i++) {
            std::cout << buffer[i];
        }

        empty_buffer(buffer);

        std::cout << std::endl << "End of Packet " << ++packetCount << std::endl;
    }

    file_in.close();

    return 0;
}


void empty_buffer(char buffer[]) {
    for (int i = 0; i < SEGMENT_SIZE; i++) {
        buffer[i] = '\0';
    }
}