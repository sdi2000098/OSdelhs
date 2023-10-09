#include "linked_hash.h"
#include <iostream>
#include <math.h>

#define ERROR -1
using namespace std;

static int KeysPerBucket =0;
class HashTable;
static HashTable * MyHash = NULL;

class Bucket {
    public :
        Item ** Records;                    
        //An array of indexes to Items, when this array reaches KeysPerBucket indexes, a new bucket is created and linked with the current bucket via NextBucker
        Bucket * NextBucket;        //Pointer to next bucket
        int ItemsStored;            //#items stored in bucket
        Bucket() : ItemsStored(0){
            Records = new Item*[KeysPerBucket];
            for (int i = 0 ; i < KeysPerBucket ; i++ )
                Records[i] = NULL;
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
        Bucket * Split();            //Will be defined later
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
        int Change(int Pin){
            for(int i = 0 ; i< KeysPerBucket;i++){
                if(Records[i] == NULL)
                    return ERROR;
                if (Records[i]->GetPin() == Pin)
                    return Records[i]->SetVote();
            }
            if (NextBucket == NULL)
                return ERROR;
            return NextBucket->Change(Pin);
        }
        ~Bucket(){
            for (int i = 0 ; i< KeysPerBucket;i++){
                if (Records[i] == NULL)
                    break;
                delete Records[i];
            }
            delete [] Records;
            if (NextBucket != NULL)
                delete NextBucket;
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
        HashTable() : round(0), PrevSize(2),NextSplit(0), Size(2),  TotalRecords(0) {
            HashBackets = (Bucket **) malloc(sizeof(Bucket *) * Size);
            for(int i = 0 ; i < Size ; i++ )
                HashBackets[i] = new Bucket;
            HashValue1 = (int)(pow(2,round)) * Size;
            HashValue2 = (int)(pow(2,round + 1)) * Size;
        }
        void InsertItem(int BucketPos,Item * item){
            HashBackets[BucketPos]->InsertIem(item);
        
        }
        void Split(){
            HashBackets = (Bucket**) realloc(HashBackets, sizeof(Bucket *) * ++Size);
            HashBackets[Size-1] = new Bucket;
            HashBackets[NextSplit] = HashBackets[NextSplit]->Split();
            Bucket * NextBucket = HashBackets[NextSplit]->NextBucket, * PrevBucket = HashBackets[NextSplit];
            if (NextBucket != NULL){
                do
                {
                    if(NextBucket->ItemsStored == 0){
                        PrevBucket->NextBucket = NULL;
                        delete NextBucket;
                        break;
                    }
                    else{
                        PrevBucket = NextBucket;
                        NextBucket = NextBucket->NextBucket;
                    }
                } while (NextBucket != NULL);
            }
            if (Size == 2*PrevSize){
                HashValue1 *=2 ;
                HashValue2 *= 2;
                round++;
                NextSplit = 0;
                PrevSize = Size;
                
            }
            else
                NextSplit++;
            
            
        }
        void PrintAll(void){
            printf("We are in size      %d    h1 = %d      h2 = %d\n",Size,HashValue1,HashValue2);
            for (int i = 0;i<Size;i++){
                printf("for bucket %d we have : ",i);
                Bucket * temp = HashBackets[i];
                while (temp != NULL)
                {
                    for (int j = 0;j<KeysPerBucket;j++){
                        if (temp->Records[j]!=NULL)
                        {
                            cout << temp->Records[j]->GetPin() << "  ";
                        }
                        
                    }
                    temp=temp->NextBucket;
                }
                cout <<"\n";
            }
            cout << "#########################################\n";
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
            free(HashBackets);
        }
};

Bucket * Bucket::Split(){
    Item * temp;
    ItemsStored = 0;
    for (int i = 0; i< KeysPerBucket;i++){
        temp = Records[i];
        if (temp == NULL)
            break;
        int ItemPos = temp->GetPin() % MyHash->HashValue2;
        Records[i] =NULL;
        MyHash->InsertItem(ItemPos,temp);
    }
    if(NextBucket != NULL)
        NextBucket = NextBucket->Split();
    return this;
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
    MyHash->TotalRecords++;
    double L = (double)MyHash->TotalRecords/ (double)(MyHash->Size * KeysPerBucket);
    if (L > 0.75)
        MyHash->Split();    
}

int Find(int Pin){          // Returns 0 if record is displayed ERROR otherwise
    int ItemPos = Pin % MyHash->HashValue1;
    if (ItemPos < MyHash->NextSplit)
        ItemPos = Pin % MyHash->HashValue2;
    int RetrurnValue = MyHash->Find(ItemPos,Pin);
    if (RetrurnValue == ERROR){
        cout << "Participant " << Pin << " not in cohort\n";
        return ERROR;
    }
    return 0;
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
    if (MyHash->Change(ItemPos,Pin) == ERROR)
        return ERROR;
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
