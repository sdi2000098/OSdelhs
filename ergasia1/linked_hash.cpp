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
        Bucket * NextBucket;
        int ItemsStored;
        Bucket() : ItemsStored(0){
            Records = (Item**)malloc(KeysPerBucket*sizeof(Item*));
            NextBucket = NULL;
        }
        void InsertIem(Item * item){
            if (ItemsStored == KeysPerBucket){
                if (NextBucket == NULL)
                    NextBucket = new Bucket;
                NextBucket->InsertIem(item);
            }
            else{
                Records[ItemsStored++] = item;
            }
        }
        void Split();
        int Find(int Pin){
            for(int i = 0 ; i< KeysPerBucket;i++){
                if(Records[i] == NULL)
                    return ERROR;
                if (Records[i]->GetPin() == Pin){
                    Records[i]->DisplayVoter();
                    return 0;
                }
            }
            if (NextBucket == NULL)
                return ERROR;
            return NextBucket->Find(Pin);
        }
        int Change(int Pin){
            for(int i = 0 ; i< KeysPerBucket;i++){
                if(Records[i] == NULL)
                    return ERROR;
                if (Records[i]->GetPin() == Pin){
                    Records[i]->SetVote();
                    AddVote(Records[i]->GetZip());
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
        int PrevSize;
        Bucket ** HashBackets;
    public : 
        int NextSplit;
        int Size;
        int HashValue1;
        int HashValue2;
        int TotalRecords;
        HashTable() : round(0),NextSplit(0),Size(2), TotalRecords(0),PrevSize(2){
            HashBackets = (Bucket **) malloc(sizeof(Bucket *) * Size);
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


int Initialize (int x){
    if (x <= 0){
        cout << "Keys per Bucket must be an integer greater than zero, youj inserted " << x << "\n";
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

int ChangeItem(int Pin){
    int ItemPos = Pin % MyHash->HashValue1;
    if (ItemPos < MyHash->NextSplit)
        ItemPos = Pin % MyHash->HashValue2;
    return MyHash->Change(ItemPos,Pin);
}

void ExitHash(void){
    delete MyHash;
}