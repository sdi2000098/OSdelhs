#include <iostream>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "Shared.h"

using namespace std;

int main (int argc, char * argv[]){
    if (argc != 9){     //Since our program only accpets calls as described in the assignment
        cout << "Wrong number of arguments!\n";
        return ERROR;
    }
    char * dataFile=NULL, *recs = NULL, *timeString = NULL, *shmidString=NULL; 
    int recid1, recid2;
    for (int i = 1 ; i < argc ; i +=2){
        //Simple reading of the arguments
        if(strcmp(argv[i],"-f") == 0 ){
            if ( ( dataFile = (char*)malloc(sizeof(char)*strlen(argv[i+1])+1) ) ==NULL){
                cout << "Could not allocate memory\n";
                return ERROR;
            }
            strcpy(dataFile,argv[i+1]);
        }
        else if(strcmp(argv[i],"-l") == 0 ){
            if ( ( recs = (char*)malloc(sizeof(char)*strlen(argv[i+1])+1) ) ==NULL){
                cout << "Could not allocate memory\n";
                return ERROR;
            }
            strcpy(recs,argv[i+1]);
            sscanf(recs,"%d,%d",&recid1,&recid2);
        }
        else if(strcmp(argv[i],"-d") == 0 ){
            if ( ( timeString = (char*)malloc(sizeof(char)*strlen(argv[i+1])+1) ) ==NULL){
                cout << "Could not allocate memory\n";
                return ERROR;
            }
            strcpy(timeString,argv[i+1]);
        }
        else if(strcmp(argv[i],"-s") == 0 ){
            if ( ( shmidString = (char*)malloc(sizeof(char)*strlen(argv[i+1])+1) ) ==NULL){
                cout << "Could not allocate memory\n";
                return ERROR;
            }
            strcpy(shmidString,argv[i+1]);
        }
        else{
            //Invalid flag
            cout << "Invalid input as argument\n";
            return ERROR;
        }
    }
    //Open shared memory
    int FileDescriptor = open(dataFile, O_RDWR);
    if (FileDescriptor == -1){
        cout << "open failed" << endl;
        return ERROR;
    }

    int shm_fd = shm_open(shmidString, O_RDWR, 0666);
    if (shm_fd == -1){
        cout << "shm_open failed" << endl;
        return ERROR;
    }
    int size = sizeof(SharedData);
    SharedData * sharedData = (SharedData *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (sharedData == MAP_FAILED){
        cout << "mmap failed" << endl;
        return ERROR;
    }
    LockToCheck LocksToCheck[MAX_PROCESSES*2];
    int NumberOfLocksToCheck;
    for (int i = 0 ;i < 2 ;i++){
        //Add a lock to shared memoery
        sem_wait(&sharedData->semaphore);
        NumberOfLocksToCheck = 0;
        int j,LockPosition=ERROR;
        for ( j = 0 ; j < MAX_PROCESSES*2 ;j++){
            if (!sharedData->SharedRangeLocks[j].valid && LockPosition == ERROR){
                sharedData->SharedRangeLocks[j].valid = true;
                sharedData->SharedRangeLocks[j].startingId = recid1;
                sharedData->SharedRangeLocks[j].endingId = recid2;
                sharedData->SharedRangeLocks[j].WriterReader = READER;
                sharedData->SharedRangeLocks[j].NumberOfBlocked = 0;
                sharedData->SharedRangeLocks[j].ProcessLockingLeft = false;
                sharedData->SharedRangeLocks[j].timesChanged++;
                sem_init(&sharedData->SharedRangeLocks[j].semaphore, 1, 0);
                LockPosition = j;
            }
            else if (sharedData->SharedRangeLocks[j].valid && sharedData->SharedRangeLocks[j].WriterReader == WRITER){
                LocksToCheck[NumberOfLocksToCheck].Index = j;
                LocksToCheck[NumberOfLocksToCheck++].TimesChanged = sharedData->SharedRangeLocks[j].timesChanged;
            }
            
        }
        if (LockPosition==MAX_PROCESSES*2){
            cout << "Too many locks\n";
            i --;
            continue;
        }
        cout << "Reader " << getpid() << " created lock  " << LockPosition << endl;
        for (int j = 0 ; j < NumberOfLocksToCheck ; j++){
            if (LocksToCheck[j].Index == LockPosition)
                continue;
            if (LocksToCheck[j].TimesChanged != sharedData->SharedRangeLocks[LocksToCheck[j].Index].timesChanged)
                continue;
            if (sharedData->SharedRangeLocks[LocksToCheck[j].Index].valid && sharedData->SharedRangeLocks[LocksToCheck[j].Index].WriterReader == WRITER){
                if (sharedData->SharedRangeLocks[LocksToCheck[j].Index].ProcessLockingLeft)
                    continue;
                if (sharedData->SharedRangeLocks[LocksToCheck[j].Index].startingId >= recid1 && sharedData->SharedRangeLocks[LocksToCheck[j].Index].startingId <= recid2){
                    sharedData->SharedRangeLocks[LocksToCheck[j].Index].NumberOfBlocked++;
                    sem_post(&sharedData->semaphore);
                    cout << "Reader " << getpid() << " blocked by lock " << LocksToCheck[j].Index <<  endl;
                    sem_wait(&sharedData->SharedRangeLocks[LocksToCheck[j].Index].semaphore);
                    sem_wait(&sharedData->semaphore);
                    sharedData->SharedRangeLocks[LocksToCheck[j].Index].NumberOfBlocked--;
                    if (sharedData->SharedRangeLocks[LocksToCheck[j].Index].NumberOfBlocked == 0){
                        sharedData->SharedRangeLocks[LocksToCheck[j].Index].valid = false;
                        sharedData->SharedRangeLocks[LocksToCheck[j].Index].ProcessLockingLeft = false;
                    }
                }
            }
        }
        cout << "Reader " << getpid() << " passed all blocks\n";
        cout.flush();
        sharedData->numReaders++;
        sharedData->ActiveReaders[sharedData->numReaders-1] = getpid();
        sem_post(&sharedData->semaphore);
        cout << "Reader " << getpid() << " is reading\n";
        double average;
        for (int j = recid1 ; j <= recid2 ; j++){
            MyRecord record;
            pread(FileDescriptor, &record, sizeof(MyRecord), j*sizeof(MyRecord));
            average += record.balance/((double)(recid2-recid1+1));
        }
        sleep(atoi(timeString));
        sem_wait(&sharedData->semaphore);
        cout << "Reader " << getpid() << "is going to Post lock " << LockPosition << " " << sharedData->SharedRangeLocks[LockPosition].NumberOfBlocked <<"times"<< endl;
        for (int j = 0 ; j < sharedData->SharedRangeLocks[LockPosition].NumberOfBlocked ; j++){
            sem_post(&sharedData->SharedRangeLocks[LockPosition].semaphore);
        }
        sharedData->SharedRangeLocks[LockPosition].ProcessLockingLeft = true;
        //Remove process from active readers
        for (int j = 0 ; j <sharedData->numReaders  ; j++){
            if (sharedData->ActiveReaders[j] == getpid()){
                for (int k = j ; k < sharedData->numReaders-1 ; k++){
                    sharedData->ActiveReaders[k] = sharedData->ActiveReaders[k+1];
                }
                break;
            }
        }
        sharedData->numReaders--;
        sem_post(&sharedData->semaphore);
    }
}