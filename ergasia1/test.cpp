// Online C++ compiler to run C++ program online
#include <iostream>
#include <string.h>
#include <fstream>
#define STRING_LENGTH 100

using namespace std;
int main(void){

    ifstream file;
    file.open("voters50.csv");
    char * s1 = (char*)malloc(STRING_LENGTH*sizeof(char));
    int i = 0,pin,zip;
    char * name = (char*) malloc (sizeof(char)*STRING_LENGTH), *surname =(char*) malloc(sizeof(char)*STRING_LENGTH);
    if (file.is_open() ){
        while( file.good()){
            
            zip = -1;
            pin = -1;
            file >> s1;
            pin = atoi(s1);
            file >> name;
            file >> surname;
            file >> s1;
            zip = atoi(s1);
            if (file.good())
                i++;
        }
    }
    cout << i <<"\n";
    return 0;
}