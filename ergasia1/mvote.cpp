#include <iostream>
#include <fstream>
#include "voters.h"

#define ERROR -1
#define STRING_LENGTH 100

using namespace std;

int main(int argc, char **argv){
    if (argc != 3){
        cout << "Wrong number of arguments!\n";
        return ERROR;
    }
    if (Initialize)
        return ERROR;
    ifstream file;
    file.open(argv[1]);
    char * s1 = (char*)malloc(STRING_LENGTH*sizeof(char));
    if (file.is_open() ){
        while( file.good()){
            file >> s1;
            cout << s1 << " ";
        }
    }
    return 0;
}
