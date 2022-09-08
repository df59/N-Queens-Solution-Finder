#include <ios>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <string>
#include <type_traits>

int* getInput(int* array);
int getFileSize();
int* getRawInput(int*);
int resizeArray();
int* fillResized(int*, int*, int);

int main() {
    int File_Size = getFileSize();
    std::cout << "file size is " << File_Size << " characters \n";

    int a[File_Size];
    int *b = getRawInput(a);
    int n = resizeArray();
    std::cout << "new size is " << n << '\n';

    int q[n];
    int *r = fillResized(b, q, n);

    std::cout << "from function 'main': \n";
    for(int i = 0; i<n; i++) {
        std::cout << "position " << i << " is the number " << r[i] << "\n";
    }

}

int getFileSize() {
    //opening file
    std::cout << "Opening input file \n";
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

    std::cout << "Closing input file \n";
    in_File.close();
    return int(file_Size);
}

int* getRawInput(int input[]) {
    //opening file
    std::cout << "Opening input file \n";
    std::string csv_File = "input.csv";  //input file must be within project directory
    std::ifstream in_File;
    in_File.open(csv_File.c_str(), std::ios::in);

    if(!in_File.is_open()) {
        std::cout << "File did not open. Make sure 'input.csv' is in the project directory. \n";
    }

    in_File.seekg(0);
    int position = 0;
    char c;
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
    return input;
}



int resizeArray() {

    //opening file
    std::cout << "Opening input file \n";
    std::string csv_File = "input.csv";  //input file must be within project directory
    std::ifstream in_File;
    in_File.open(csv_File.c_str(), std::ios::in);

    if(!in_File.is_open()) {
        std::cout << "File did not open. Make sure 'input.csv' is in the project directory. \n";
    }

    in_File.seekg(0);
    int new_Size = 0;
    char c;
    while(in_File.get(c)) {
    if((int(c) - int('0')) < 10) {
        if((int(c) - int('0')) >=0) {
    new_Size++;
        }
    }
    }

    return new_Size;
}

int* fillResized(int old[], int resized[], int new_Size) {
    //std::cout << "from function 'fillResized': \n";
    for(int i = 0; i < new_Size; i++) {
    resized[i] = old[i];
    //std::cout << "position " << i << " is the number " << resized[i] << "\n";

    }
    return resized;
}