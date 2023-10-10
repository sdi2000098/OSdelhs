#include <cstring>
#include <iostream>
#include "linked_hash.h"
#define ERROR -1
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
            cout << Pin << " " << surname << " " << name << " " << PostCode << " " ;
            if (HasVoted == true)
                cout << "Y\n";
            else
                cout << "N\n";
        }

        int Voter::GetZip(){
            return PostCode;
        }
        int Voter::SetVote(){
            if (HasVoted){
                cout << "Canditate with pin " << Pin << "has already voted\n";
                return ERROR;
            }
            cout << Pin << " marked voted\n";
            HaveVoted++;
            HasVoted = true;
            return 0;
        }
    
    typedef Voter Item;

    int Initialize (int KeysPerBucket, int InitialSize){
        return InitializeHash(KeysPerBucket,InitialSize);
    }
    int CreateVoter(int Pin, char * surname, char * name, int PostCode){
        if (FindRecord(Pin) != NULL)
            return ERROR;
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
        int BytesDeleted = ExitHash();
        BytesDeleted+= ExitList();
        return BytesDeleted;
    }
