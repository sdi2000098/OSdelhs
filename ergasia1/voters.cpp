#include "voters.h"
#include <cstring>
#include <iostream>
using namespace std;

    static int VotersNumber = 0, HaveVoted = 0;
    


    class Voter
    {
        public:
            Voter(int Pin, const char * surname, const char * name, int PostCode) 
            :Pin(Pin), PostCode(PostCode), HasVoted(false)
            {
                this->name = new char[strlen(name) + 1];
                strcpy(this->name, name);
                this->surname = new char[strlen(surname) + 1];
                strcpy(this->surname, surname);

            }
            ~Voter(){
                delete []name;
                delete []surname;
            }

            int GetPin(){
                return Pin;
            }
            
            void DisplayVoter(){
                cout << "Pin : " << Pin;
                cout << "Name : " << name;
                cout << "Surname : " << surname;
                cout << "Post Code : " << PostCode;
                cout << "Voted : " << HasVoted;
            }

            void SetVote(){
                if (HasVoted){
                    cout << "Already Voted";
                    return;
                }
                HaveVoted++;
                HasVoted = true;
                return;
            }
        private : 
            int Pin,PostCode;
            char *name,*surname;
            bool HasVoted;
    };

    

    int Initialize (int KeysPerBucket){
        
        return 0;
    }
    int CreateVoter(int Pin, char * surname, char * name, int PostCode){
        VotersNumber++;
    }

    int PrintVoter(int Pin){

    }

    int SetVoted(int Pin){

    }

    int NumberOfVoters (void){
        return VotersNumber;
    }

    int NumberOfYesVoters (void){
        return HaveVoted;
    }

    int PrintPostalCodes (void){

    }

    int Exit(void){

    }
