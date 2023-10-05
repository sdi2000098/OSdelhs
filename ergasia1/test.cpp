// Online C++ compiler to run C++ program online
#include <iostream>
#include <string.h>
#include <fstream>
#define STRING_LENGTH 100

using namespace std;
int main(int argc, char **argv){
    if (argc != 5){
        cout << "Wrong number of arguments!\n";
    }
    ifstream file;
    file.open(argv[2]);
    char * s1 = (char*)malloc(STRING_LENGTH*sizeof(char));
    if (file.is_open() ){
        while( file.good()){
            file >> s1;
            cout << s1 << "\n";
        }
    }
    return 0;
}