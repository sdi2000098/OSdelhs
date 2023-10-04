#ifndef LINKED_HASH

    #define LINKED_HASH
    #include "voters.h"
    
    typedef Voter Item;
    int Initialize(int KeysPerBucket);
    int Insert(Item * item);
    int Find(int Pin);
    int ChangeItem(int Pin);
    int Exit(void);
#endif 