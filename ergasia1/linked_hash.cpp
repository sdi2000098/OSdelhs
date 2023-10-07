#include "linked_hash.h"
#include <iostream>
#include <math.h>

#define ERROR -1
using namespace std;

static int KeysPerBucket;
class HashTable;
static HashTable * MyHash;

class Bucket {
    public :
        Item ** Records;                    
        //An array of indexes to Items, when this array reaches KeysPerBucket indexes, a new bucket is created and linked with the current bucket via NextBucker
        Bucket * NextBucket;        //Pointer to next bucket
        int ItemsStored;            //#items stored in bucket
        Bucket() : ItemsStored(0){
            Records = (Item**)malloc(KeysPerBucket*sizeof(Item*));
            NextBucket = NULL;
        }
        void InsertIem(Item * item){
            if (ItemsStored == KeysPerBucket){            //Bucket is full
                if (NextBucket == NULL)                   //if there is no next bucket, create one
                    NextBucket = new Bucket;
                NextBucket->InsertIem(item);                //Insert the item to the bucket created
            }
            else{
                Records[ItemsStored++] = item;              //Bucket is not full, insert to the free space
            }
        }
        void Split();            //Will be defined later
        int Find(int Pin){
            for(int i = 0 ; i< KeysPerBucket;i++){       //Check all records in the bucket
                if(Records[i] == NULL)
                    return ERROR;
                if (Records[i]->GetPin() == Pin){
                    Records[i]->DisplayVoter();
                    return 0;
                }
            }
            if (NextBucket == NULL)
                return ERROR;
            return NextBucket->Find(Pin);    // If not foun yet check next bucket
        }
        Item * FindRecord( int Pin){         // Function that given pin returns item
            //This is needed when vote is changed to yes
            //given pin the appropriate item needs to be inserted to the inverted list
            for(int i = 0 ; i< KeysPerBucket;i++){
                if(Records[i] == NULL)
                    return NULL;
                if (Records[i]->GetPin() == Pin){
                    return Records[i];
                }
            }
            if (NextBucket == NULL)
                return NULL;
            return NextBucket->FindRecord(Pin);
        }
        int Change(int Pin){  //Changes vote to yes
            for(int i = 0 ; i< KeysPerBucket;i++){
                if(Records[i] == NULL)
                    return ERROR;
                if (Records[i]->GetPin() == Pin){
                    Records[i]->SetVote();
                    return 0;
                }
            }
            if (NextBucket == NULL)
                return ERROR;
            return NextBucket->Find(Pin);
        }
        ~Bucket(){
            for (int i = 0 ; i < KeysPerBucket;i++){
                if (Records[i] == NULL)
                    break;
                delete Records[i];
            }
            if (NextBucket != NULL)
                delete NextBucket;
            delete [] Records;
        }
};

class HashTable {  
    private: 
        int round;
        int PrevSize;  //This variable indicates the previous size of the table in which hash functions changed value 
        Bucket ** HashBackets;  //Array of hash buckets 
    public : 
        int NextSplit;     //Points to the bucket that is going to be split if needed 
        int Size;       // Current size of hash table
        int HashValue1;     //h_i
        int HashValue2;     //h_i+1
        int TotalRecords;
        HashTable() : round(0), PrevSize(2),NextSplit(0), Size(2),  TotalRecords(0) {
            //We begine at round zero and size equal to 2
            HashBackets = (Bucket **) malloc(sizeof(Bucket *) * Size);
            // Number of buckets is defined by hash table size
            for(int i = 0 ; i < Size ; i++ )
                HashBackets[i] = new Bucket;
            HashValue1 = (int)(pow(2,round)) * Size;
            HashValue2 = (int)(pow(2,round + 1)) * Size;
        }
        void InsertItem(int BucketPos,Item * item){
            HashBackets[BucketPos]->InsertIem(item);
            TotalRecords++;
        }
        void Split(){
            HashBackets = (Bucket**) realloc(HashBackets, sizeof(Bucket *) * ++Size);
            HashBackets[NextSplit]->Split();
            Bucket * NextBucket = HashBackets[NextSplit]->NextBucket, * PrevBucket = HashBackets[NextSplit],*temp;
            if (NextBucket != NULL){
                do
                {
                    if(NextBucket->ItemsStored == 0){
                        if (PrevBucket != NULL)
                            PrevBucket->NextBucket = NULL;
                        temp = NextBucket->NextBucket;
                        delete NextBucket;
                        NextBucket = temp;
                        PrevBucket = NULL;
                    }
                    else{
                        PrevBucket = NextBucket;
                        NextBucket = NextBucket->NextBucket;
                    }

                } while (NextBucket != NULL);
                
            }
            if (Size == 2*PrevSize){
                round++;
                NextSplit = 0;
                PrevSize = Size;
            }
            else
                NextSplit++;
            HashValue1 = (int)(pow(2,round)) * Size;
            HashValue2 = (int)(pow(2,round + 1)) * Size;
        }
        int Find(int ItemPos, int Pin){
            return HashBackets[ItemPos]->Find(Pin);
        }
        Item * FindRecord(int ItemPos, int Pin){
            return HashBackets[ItemPos]->FindRecord(Pin);
        }
        int Change(int ItemPos, int Pin){
            return HashBackets[ItemPos]->Change(Pin);
        }
        ~HashTable(){
            for(int i = 0 ; i< Size; i++)
                delete HashBackets[i];
            delete [] HashBackets;
        }
};

void Bucket::Split(){
    Item * temp;
    ItemsStored = 0;
    for (int i = 0; i< KeysPerBucket;i++){
        temp = Records[i];
        if (temp == NULL)
            break;
        Records[i] =NULL;
        int ItemPos = temp->GetPin() % MyHash->HashValue1;
        MyHash->InsertItem(ItemPos,temp);
    }
    if(NextBucket != NULL)
        NextBucket->Split();
}


int InitializeHash (int x){
    if (x <= 0){
        cout << "Keys per Bucket must be an integer greater than zero, you inserted " << x << "\n";
        return ERROR;
    }
    KeysPerBucket = x;
    MyHash = new HashTable;
    return 0;
}

void Insert (Item  * item) {
    int ItemPos = item->GetPin() % MyHash->HashValue1;
    if (ItemPos < MyHash->NextSplit)
        ItemPos = item->GetPin() % MyHash->HashValue2;
    MyHash->InsertItem(ItemPos,item);
    double L = (double)MyHash->TotalRecords/ (double)(MyHash->Size * KeysPerBucket);
    if (L > 0.75)
        MyHash->Split();
}

int Find(int Pin){          // Returns 0 if record is displayed ERROR otherwise
    int ItemPos = Pin % MyHash->HashValue1;
    if (ItemPos < MyHash->NextSplit)
        ItemPos = Pin % MyHash->HashValue2;
    return MyHash->Find(ItemPos,Pin);
}
Item * FindRecord(int Pin){
    int ItemPos = Pin % MyHash->HashValue1;
    if (ItemPos < MyHash->NextSplit)
        ItemPos = Pin % MyHash->HashValue2;
    return MyHash->FindRecord(ItemPos,Pin);
}
int ChangeItem(int Pin){
    int ItemPos = Pin % MyHash->HashValue1;
    if (ItemPos < MyHash->NextSplit)
        ItemPos = Pin % MyHash->HashValue2;
    MyHash->Change(ItemPos,Pin);
    Item * ToInsert = FindRecord(Pin);
    if (ToInsert == NULL){
        cout << "There is no candiatte with Pin : " << Pin << "\n";
        return ERROR;
    }
    InsertList(ToInsert);
    return 0;
}

void ExitHash(void){
    delete MyHash;
}
