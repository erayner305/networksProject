// This program is intended to be run on the client side. It will initiate connection with a server and push a file using segmentation

#include <iostream>
#include <fstream>
#include <vector>

#define SEGMENT_SIZE 512

void empty_buffer(char buffer[]);

int gremlins(char buffer[], double corruptionChance, double lossChance)

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

int gremlins(char buffer[], double corruptionChance, double lossChance){
    double randomNum;
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
            buffer[8] = '\0';
        }
        
        if(randomNum <= 0.2){ //20% chance two packets are affected
            buffer[9] = '\0';
        }

        if(randomNum <= 0.1){ //10% chance three packets are affected
            buffer[10] = '\0';
        }
        return 2;
    }

    return 0;
 
}