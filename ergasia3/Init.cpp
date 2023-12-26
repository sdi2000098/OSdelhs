#include <iostream>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include "Shared.h"
#include <cstring>    // For memset

#define ERROR -1
#define READER 0
#define WRITER 1

using namespace std;

int main(void){
    const char *shmName = "/SharedMemory";
    int shm_fd = shm_open(shmName, O_CREAT | O_RDWR | O_EXCL, 0666);
    if (shm_fd == -1){
        if (errno == EEXIST) { // Shared memory already exists
            shm_unlink(shmName);
            shm_fd = shm_open(shmName, O_CREAT | O_RDWR, 0666);
            if (shm_fd == -1) {
                std::cout << "shm_open failed" << std::endl;
                return ERROR;
            }
        } else {
            std::cout << "shm_open failed" << std::endl;
            return ERROR;
        }
    }
    int size = sizeof(SharedData);
    if (ftruncate(shm_fd, size) == -1){
        std::cout << "ftruncate failed" << std::endl;
        return ERROR;
    }
    SharedData * sharedData = (SharedData *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (sharedData == MAP_FAILED){
        std::cout << "mmap failed" << std::endl;
        return ERROR;
    }
    memset(sharedData, 0, sizeof(SharedData));
    sharedData->numReaders = 0;
    sharedData->numWriters = 0;
    sharedData->numRecordsProecessed = 0;
    sem_init(&sharedData->semaphore, 1, 1);
    for (int i = 0 ; i < MAX_PROCESSES*2 ; i++){
        sharedData->SharedRangeLocks[i].valid = false;
        sharedData->SharedRangeLocks[i].ProcessLockingLeft = false;
        sharedData->SharedRangeLocks[i].timesChanged=0;
    }
    int pid;
    for (int i = 0 ; i < 5 ; i++){
        pid = fork();
        if (pid == 0){
            char int_str[SIZEofBUFF];
            sprintf(int_str, "%d,%d",0,5);
            execl("./reader", "./reader", "-f", "accounts50.bin","-l", int_str, "-d", "2", "-s", "/SharedMemory", NULL);
        }
    }
    for (int i = 0 ; i < 5 ; i++){
        pid = fork();
        if (pid == 0){
            char int_str[SIZEofBUFF];
            sprintf(int_str, "%d",3);
            if ( execl("./writer", "./writer", "-f", "accounts50.bin","-l", int_str, "-d", "2", "-s", "/SharedMemory","-v","10", NULL) == -1 ){
                perror("execl");
            }
        }
    }
    // Wait for all children to finish
    for (int i = 0 ; i < 10 ; i++){
        wait(NULL);
    }
    // Unmap shared memory
    if (munmap(sharedData, size) == -1){
        std::cout << "munmap failed" << std::endl;
        return ERROR;
    }
    // Close shared memory
    if (close(shm_fd) == -1){
        std::cout << "close failed" << std::endl;
        return ERROR;
    }
    // Unlink shared memory
    if (shm_unlink("/SharedMemory") == -1){
        std::cout << "shm_unlink failed" << std::endl;
        return ERROR;
    }
    return 0;

    
}