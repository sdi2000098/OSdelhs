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
//Program for the writer
int main (int argc, char * argv[]){
    // Print all arguments

    if (argc != 11){     //Since our program only accpets calls as described in the assignment
        cout << "Wrong number of arguments!\n";
        return ERROR;
    }
    char * dataFile=NULL, *recs = NULL, *timeString = NULL, *shmidString=NULL, *valueString=NULL; 
    int recid;
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
            recid = atoi(argv[i+1]);
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
        else if(strcmp(argv[i],"-v") == 0 ){
            if ( ( valueString = (char*)malloc(sizeof(char)*strlen(argv[i+1])+1) ) ==NULL){
                cout << "Could not allocate memory\n";
                return ERROR;
            }
            strcpy(valueString,argv[i+1]);
        }
        else{
            //Invalid flag
            cout << "Invalid input as argument\n";
            return ERROR;
        }
    }
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
        NumberOfLocksToCheck = 0;
        sem_wait(&sharedData->semaphore);
        int j,LockPosition=ERROR;
        for ( j = 0 ; j < MAX_PROCESSES*2 ;j++){
            if (!sharedData->SharedRangeLocks[j].valid && LockPosition == ERROR){
                sharedData->SharedRangeLocks[j].valid = true;
                sharedData->SharedRangeLocks[j].startingId = recid;
                sharedData->SharedRangeLocks[j].endingId = recid;
                sharedData->SharedRangeLocks[j].WriterReader = WRITER;
                sharedData->SharedRangeLocks[j].NumberOfBlocked = 0;
                sharedData->SharedRangeLocks[j].ProcessLockingLeft = false;
                sharedData->SharedRangeLocks[j].timesChanged++;
                sem_init(&sharedData->SharedRangeLocks[j].semaphore, 1, 0);
                LockPosition = j;
            }
            else if (sharedData->SharedRangeLocks[j].valid  ){
                LocksToCheck[NumberOfLocksToCheck].Index = j;
                LocksToCheck[NumberOfLocksToCheck++].TimesChanged = sharedData->SharedRangeLocks[j].timesChanged;
            }
        }
        if (LockPosition==MAX_PROCESSES*2){
            cout << "Too many locks\n";
                i --;
                continue;
            }
        cout << "Writer " << getpid() << " created lock " << LockPosition << endl;
        for (int j = 0;j<NumberOfLocksToCheck ; j++){
            if (LocksToCheck[j].Index == LockPosition)
                continue;
            if (LocksToCheck[j].TimesChanged != sharedData->SharedRangeLocks[LocksToCheck[j].Index].timesChanged)
                continue;
            if (sharedData->SharedRangeLocks[LocksToCheck[j].Index].valid && sharedData->SharedRangeLocks[j].WriterReader == WRITER){
                if (sharedData->SharedRangeLocks[LocksToCheck[j].Index].ProcessLockingLeft)
                    continue;
                if (sharedData->SharedRangeLocks[LocksToCheck[j].Index].startingId == recid ){
                    sharedData->SharedRangeLocks[LocksToCheck[j].Index].NumberOfBlocked++;
                    sem_post(&sharedData->semaphore);
                    cout << "Writer " << getpid() << " blocked by lock " << LocksToCheck[j].Index << endl;
                    sem_wait(&sharedData->SharedRangeLocks[LocksToCheck[j].Index].semaphore);
                    sem_wait(&sharedData->semaphore);
                    sharedData->SharedRangeLocks[LocksToCheck[j].Index].NumberOfBlocked--;
                    if (sharedData->SharedRangeLocks[LocksToCheck[j].Index].NumberOfBlocked == 0){
                        sharedData->SharedRangeLocks[LocksToCheck[j].Index].ProcessLockingLeft = false;
                        sharedData->SharedRangeLocks[LocksToCheck[j].Index].valid = false;
                    }
                }
            }
            else if (sharedData->SharedRangeLocks[LocksToCheck[j].Index].valid && sharedData->SharedRangeLocks[LocksToCheck[j].Index].WriterReader == READER){
                if (sharedData->SharedRangeLocks[LocksToCheck[j].Index].ProcessLockingLeft)
                    continue;
                if (sharedData->SharedRangeLocks[LocksToCheck[j].Index].startingId <= recid && sharedData->SharedRangeLocks[LocksToCheck[j].Index].endingId >= recid){
                    sharedData->SharedRangeLocks[LocksToCheck[j].Index].NumberOfBlocked++;
                    sem_post(&sharedData->semaphore);
                    cout << "Writer " << getpid() << " blocked by lock " << LocksToCheck[j].Index << endl;
                    sem_wait(&sharedData->SharedRangeLocks[LocksToCheck[j].Index].semaphore);
                    sem_wait(&sharedData->semaphore);
                    sharedData->SharedRangeLocks[LocksToCheck[j].Index].NumberOfBlocked--;
                    if (sharedData->SharedRangeLocks[LocksToCheck[j].Index].NumberOfBlocked == 0){
                        sharedData->SharedRangeLocks[LocksToCheck[j].Index].ProcessLockingLeft = false;
                        sharedData->SharedRangeLocks[LocksToCheck[j].Index].valid = false;
                    }
                }
            }
        }
        cout << "Writer " << getpid() << " passed all blocks\n";
        sharedData->numWriters++;
        sharedData->ActiveWriters[sharedData->numWriters-1] = getpid();
        sem_post(&sharedData->semaphore);
        //Write to the file
        cout << "Writer " << getpid() << " is writing\n";
        MyRecord record;
        pread(FileDescriptor, &record, sizeof(MyRecord), recid*sizeof(MyRecord));
        record.balance += atoi(valueString);
        pwrite(FileDescriptor, &record, sizeof(MyRecord), recid*sizeof(MyRecord));
        sleep(atoi(timeString));
        sem_wait(&sharedData->semaphore);
        cout << "Writer " << getpid() << "is going to Post lock " << LockPosition << " " << sharedData->SharedRangeLocks[LockPosition].NumberOfBlocked <<" times"<< endl;
        for (int j = 0 ; j < sharedData->SharedRangeLocks[LockPosition].NumberOfBlocked ; j++){
            sem_post(&sharedData->SharedRangeLocks[LockPosition].semaphore);
        }
        sharedData->SharedRangeLocks[LockPosition].ProcessLockingLeft = true;
        //Remove process from active writers
        for (int j = 0 ; j <sharedData->numWriters  ; j++){
            if (sharedData->ActiveWriters[j] == getpid()){
                for (int k = j ; k < sharedData->numWriters-1 ; k++){
                    sharedData->ActiveWriters[k] = sharedData->ActiveWriters[k+1];
                }
                break;
            }
        }
        sharedData->numWriters--;
        sem_post(&sharedData->semaphore);
    }
}