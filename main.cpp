#include <ios>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <string>

void getInput();

int main() {
    getInput();
}

void getInput() {
//opening file
    std::string csv_File = "input.csv";  //input file must be within project directory
    std::ifstream in_File;
    in_File.open(csv_File.c_str(), std::ios::in);

    if(!in_File.is_open()) {
        std::cout << "File did not open. Make sure 'input.csv' is in the project directory. \n";
    }
//finding file size to size array
    std::streampos file_Size = 0;
    file_Size = in_File.tellg();
    in_File.seekg(0, std::ios::end);
    file_Size = in_File.tellg() - file_Size;

    std::cout << "file size is " << int(file_Size) << " characters \n";

    in_File.seekg(0);
    int position = 0;
    char c;
    int input[int(file_Size)];
//filling array with raw input from file
    while(in_File.get(c)) {
    if((int(c) - int('0')) < 10) {
        if((int(c) - int('0')) >=0) {
    input[position] = (int(c) - int('0')); //subtracting ascii 0 from ascii(c) to convert ascii code to real int value
    //std::cout << "position " << position + 1 << " is the number " << input[position] << "\n";
    position++;
        }
    }
    }
//resizing input array with subtracted syntax from file
int input_Resized[position];
for(int i = 0; i < position; i++) {
    input_Resized[i] = input[i];
    std::cout << "position " << i + 1 << " is the number " << input_Resized[i] << "\n";

}    

}