#include <ios>
#include <iostream>
#include <fstream>
#include <ostream>
#include <pthread.h>
#include <string>
#include <type_traits>
#include <cmath>
#include <vector>

int getFileSize();
int* getRawInput(int*);
int resizeArray();
int* fillResized(int*, int*, int);



class Board {
    private:
    std::vector<int> state;
    public:
    Board(std::vector<int> in_Board) {
        state = in_Board;
    }

    ~Board() {
        state.clear();
        state.shrink_to_fit();
    }

    int Width() const {
        return std::sqrt(state.size());
    }

    int& operator[](unsigned int index) {
        return state[index];
    }

    friend std::ostream& operator<<(std::ostream& stream, const Board& board);


};



std::ostream& operator<<(std::ostream& stream, const Board& board) {
    //Debug std::cout << "output stream of Board.state: \n";
    for(int i = 0; i<board.Width(); i++) {
    for(int j = board.Width()*i; j<board.Width()*(i+1); j++) {
    stream << board.state[j] << " | ";
    }
    std::cout << '\n';
    }
return stream;
}



int main() {
    int File_Size = getFileSize();
    std::cout << "file size is " << File_Size << " characters \n";

    int a[File_Size];                       //allocating array in the scope of main
    int *raw_Input = getRawInput(a);
    int reduced_size = resizeArray();
    std::cout << "new size is " << reduced_size << '\n';

    int q[reduced_size];
    int *reduced_Input = fillResized(raw_Input, q, reduced_size);

    /*Debug std::cout << "from function 'main' reduced size array: \n";
    for(int i = 0; i < reduced_size; i++) {
        std::cout << "position " << i << " is the number " << reduced_Input[i] << "\n";
    }
    */

    int width = sqrt(reduced_size);
    std::vector<int> input_Vector;
    //Debug std::cout << "inputting array into std::vector: \n";
    for(int i = 0; i < reduced_size; i++) {
    input_Vector.push_back(reduced_Input[i]);
    //Debug std::cout << input_Vector[i] << '\n';
    }

    Board start_State = input_Vector;
    std::cout << "The starting board is: \n" << start_State;

    std::cout << "Board object start_State width is: \n" << start_State.Width() << '\n';

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