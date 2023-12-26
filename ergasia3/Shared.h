#ifndef MESSAGING_H
    #define MESSAGING_H
    #define ERROR -1
    #define READER 0
    #define WRITER 1
    #define MAX_PROCESSES 20
    #define SIZEofBUFF 20
    #include <semaphore.h>
    #include <time.h>

    typedef struct{
        int  	custid;
        char 	LastName[SIZEofBUFF];
        char 	FirstName[SIZEofBUFF];
        int	balance;
    } MyRecord;

    typedef struct{
        int Index;
        int TimesChanged;
    }LockToCheck;

    typedef struct{
        int startingId;
        int endingId;
        int WriterReader;
        bool valid;
        sem_t semaphore;
        int NumberOfBlocked;
        bool ProcessLockingLeft;
        int timesChanged;
    }RangeLock;

    typedef struct{
        int numReaders;
        int numWriters;
        int numRecordsProecessed;
        int ActiveReaders[MAX_PROCESSES];
        int ActiveWriters[MAX_PROCESSES];
        sem_t semaphore;
        RangeLock SharedRangeLocks[MAX_PROCESSES*2];
    }SharedData;
#endif