#include <iostream>

int main() {

}

char[] generate_checksum(char buffer[]) {
    uint32_t sum = 0;
    for(int i = 0; i < len(buffer); i++) {
        sum += buffer[i];
    }
    char result[4];
    shift_value = 32;
    for (int i = 0; i < 8; i++) { 
        result[i] = (var >> (shift_value -= 4)) & 0xF; 
    }
    return result
}