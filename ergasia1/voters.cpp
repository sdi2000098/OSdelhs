#include <cstring>
#include <iostream>
#include "linked_hash.h"


using namespace std;

    static int VotersNumber = 0, HaveVoted = 0;
    
    Voter::Voter(int Pin, const char * surname, const char * name, int PostCode) 
        :Pin(Pin), PostCode(PostCode), HasVoted(false)
        {
            this->name = new char[strlen(name) + 1];
            strcpy(this->name, name);
            this->surname = new char[strlen(surname) + 1];
            strcpy(this->surname, surname);
        }
        Voter::~Voter(){
            delete []name;
            delete []surname;
        }

        int Voter::GetPin(){
            return Pin;
        }
            
        void Voter::DisplayVoter(){
            cout << "Pin : " << Pin;
            cout << "Name : " << name;
            cout << "Surname : " << surname;
            cout << "Post Code : " << PostCode;
            cout << "Voted : " << HasVoted;
        }

        int Voter::GetZip(){
            return PostCode;
        }
        void Voter::SetVote(){
            if (HasVoted){
                cout << "Already Voted";
                return;
            }
            HaveVoted++;
            HasVoted = true;
        }
    
    typedef Voter Item;

    int Initialize (int KeysPerBucket){
        return Initialize(KeysPerBucket);
    }
    int CreateVoter(int Pin, char * surname, char * name, int PostCode){
        Item * ToInsert = new Voter(Pin,surname,name,PostCode);
        Insert(ToInsert);
        VotersNumber++;
        return 0;
    }

    int PrintVoter(int Pin){
        return Find(Pin);
    }

    int SetVoted(int Pin){
        return ChangeItem(Pin);
    }

    int NumberOfVoters (void){
        return VotersNumber;
    }

    int NumberOfYesVoters (void){
        return HaveVoted;
    }

    void PrintPostalCodes (void){
        DisplayZips();
    }

    int PrintAllFromZip(int Zip){
        return VotersFromZip(Zip);
    }
    int ExitProg(void){
        ExitHash();
        ExitList();
        return 0;
    }
