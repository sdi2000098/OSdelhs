#include <iostream>
#include <fstream>
#include "voters.h"

#define ERROR -1
#define STRING_LENGTH 100

using namespace std;

void ShowPrompt( void ){
    cout << "Please enter any of the following orders, as many times as you want to perform the corresponding task : \n ";
    cout << "1 <pin>    (Find voter with pin as id and display)\n";
    cout << "i <pin> <lname> <fname> <zip>   (Insert New Voter)\n";
    cout << "m <pin>    (Mark as voted)\n";
    cout << "bv <fileofkeys>    (Set voters of file as voted)\n";
    cout << "v    (Number of people that voted)\n";
    cout << " perc   (Percentage of people that voted)\n";
    cout << "z <zipcode>   (Voters of a specific zipcode)\n";
    cout << "o   (Display all zipcodes )\n";
    cout << "exit\n\n";

}

void Operate(void) {
    
}
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
    ShowPrompt();
    Operate();
    return 0;
}
