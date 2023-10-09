#include <iostream>
#include <fstream>
#include "voters.h"
#include <string.h>

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
    cout << "prompt (Show prompt)\n";
    cout << "exit\n";

}

void Operate(void) {
    char * Input = (char*)malloc(sizeof(char)*100);
    char **tokens =(char **) malloc(6*sizeof(char *));
    while (1)
    {
        cin.getline(Input,100);
        
        int i = 0;
        tokens[i] = strtok(Input," ");
        while (tokens[i] != NULL)
        {
            i++;
            if (i == 6){
                cout << "Too Many Arguments\n";
                break;;
            }
            tokens[i] = strtok(NULL," ");
        }
        if ( i == 6)
            continue;
        if (strcmp(tokens[0],"l") == 0){
            if ( i != 2){
                cout << "Wrong number of arguments in input\n";
                continue;
            }
            bool flag = false;
            for (int i = 0; i < (int)strlen(tokens[1]); i++) {
                if(!isdigit(tokens[1][i])){
                    cout<< "Malformed Pin\n";
                    flag = true;
                    break;;
                }
            }
            if(flag)
                continue;
            PrintVoter(atoi(tokens[1]));
        }
        else if (strcmp(tokens[0],"i") == 0)
        {
            if ( i != 5){
                cout << "Wrong number of arguments in input\n";
                continue;
            }
            bool flag = false;
            for (int i = 0; i < (int)strlen(tokens[1]); i++) {
                if(!isdigit(tokens[1][i])){
                    cout<< "Malformed Input\n";
                    flag = true;
                    break;;
                }
            }
            if(flag)
                continue;
            flag = false;
            for (int i = 0; i < (int)strlen(tokens[4]); i++) {
                if(!isdigit(tokens[4][i])){
                    cout<< "Malformed Input\n";
                    flag = true;
                    break;;
                }
            }
            if(flag)
                continue;
            
            if ( CreateVoter(atoi(tokens[1]),tokens[2],tokens[3],atoi(tokens[4])) == ERROR)
                cout << tokens[1] << " already exist\n";
            else
                cout << "Inserted" << " " << tokens[1] << " " << tokens[2] << " " << tokens[3]<< " "  << tokens[4] << " N\n" ;
        }
        else if (strcmp(tokens[0],"m") == 0)
        {
            if ( i != 2){
                cout << "Wrong number of arguments in input\n";
                continue;
            }
            bool flag = false;
            for (int i = 0; i < (int)strlen(tokens[1]); i++) {
                if(!isdigit(tokens[1][i])){
                    cout<< "Malformed Input\n";
                    flag = true;
                    break;;
                }
            }
            if(flag)
                continue;
            if ( SetVoted(atoi(tokens[1])) == ERROR)
                cout << tokens[1] << " does not exist\n";
        }
        else if (strcmp(tokens[0],"bv") == 0)
        {
            if ( i != 2){
                cout << "Wrong number of arguments in input\n";
                continue;
            }
            ifstream file;
            file.open(tokens[1]);
            if (file.is_open()){
                char * s1 = (char*)malloc(STRING_LENGTH*sizeof(char));
                while(file.good()){
                    file >> s1;
                    SetVoted(atoi(s1));
                }
                free(s1);
            }
            else
                cout << tokens[i]<< "could not be opened\n";
        }
        else if (strcmp(tokens[0],"v") == 0)
        {
            if ( i != 1){
                cout << "Wrong number of arguments in input\n";
                continue;
            }
            cout << "Total number of participants that have already voted : " << NumberOfYesVoters() << "\n";
        }
        else if (strcmp(tokens[0],"o") == 0)
        {
            if ( i != 1){
                cout << "Wrong number of arguments in input\n";
                continue;
            }
            PrintPostalCodes(); 
        }
        else if (strcmp(tokens[0],"exit") == 0)
        {
            if ( i != 1){
                cout << "Wrong number of arguments in input\n";
                continue;
            }
            free(tokens);
            cout << "Exinitng...\n";
            ExitProg();
            break;
        }
        else if (strcmp(tokens[0],"perc") == 0)
        {
            if ( i != 1){
                cout << "Wrong number of arguments in input\n";
                continue;
            }
            cout << "The percentage of those who have voted is : " << (double)NumberOfYesVoters()/(double)NumberOfVoters() << "\n";

        }
        else if (strcmp(tokens[0],"z") == 0)
        {
            if ( i != 2){
                cout << "Wrong number of arguments in input\n";
                continue;
            }
            PrintAllFromZip(atoi(tokens[1]));
        }
        else if (strcmp(tokens[0],"prompt") == 0){
            if ( i != 1){
                cout << "Wrong number of arguments in input\n";
                continue;
            }
            ShowPrompt();
        }
        else {
            cout << "Wrong input!\n";
        }
        
    }
    free(Input);
}
int main(int argc, char **argv){
    if (argc != 5){
        cout << "Wrong number of arguments!\n";
        return ERROR;
    }
    for (int i = 0; i < (int)strlen(argv[4]); i++) {
      if(! isdigit(argv[4][i])){
        cout<<"Expected an int for keys per bucket but got : " << argv[4] << "\n";
        return ERROR;
      }

   }
    
    if (Initialize(atoi(argv[4])))
        return ERROR;
    
    ifstream file;
    file.open(argv[2]);
    char * s1 = (char*)malloc(STRING_LENGTH*sizeof(char));
    if (file.is_open() ){
        int zip,pin;
        char * name = (char*) malloc (sizeof(char)*STRING_LENGTH), *surname =(char*) malloc(sizeof(char)*STRING_LENGTH);
        int i = 0 ;
        while( file.good()){
            i++;
            zip = -1;
            pin = -1;
            file >> s1;
            pin = atoi(s1);
            file >> name;
            file >> surname;
            file >> s1;
            zip = atoi(s1);
            CreateVoter(pin,surname,name,zip);
        }
        free(name);
        free(surname);
    }
    else{
        cout << "Could not open file " << argv[2] << "\n";
        return ERROR;
    }
    free(s1);
    ShowPrompt();
    Operate();
    return 0;
}
