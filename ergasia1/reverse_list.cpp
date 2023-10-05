#include "reverse_list.h"
#include <iostream>
#define ERROR -1
struct ItemNode{
    Item * item;
    ItemNode * Next;
};

struct ZipNode{
    int Zip, NumberOfItems,HaveVoted;
    ZipNode * Next;
    ItemNode * Items;
};


static int TotalZips;
static ZipNode * Head = NULL;

ZipNode * CreateNewZip(Item * item, ZipNode * Next,int Zip){
    TotalZips++;
    ZipNode * ToReturn = (ZipNode*)malloc(sizeof(ZipNode));
    ToReturn->Items = (ItemNode *)malloc(sizeof(ItemNode));
    ToReturn->Items->item = item;
    ToReturn->Items->Next = NULL;
    ToReturn->Next = Next;
    ToReturn->NumberOfItems = 1;
    ToReturn->HaveVoted = 0;
    ToReturn->Zip = Zip;
    return ToReturn;
}

void InsertList(Item * item){
    if (Head == NULL){
        TotalZips = 0;
        Head = CreateNewZip(item,NULL,item->GetZip());
        return;
    }
    ZipNode * temp = Head;
    while(temp != NULL){
        if(temp->Zip == item->GetZip()){
            temp->NumberOfItems++;
            ItemNode * prev = temp->Items;
            ItemNode * next = prev->Next;
            while (next != NULL)
            {
                prev = next;
                next = next->Next;
            }
            prev->Next = (ItemNode*)malloc(sizeof(ItemNode));
            prev->Next->item = item;
            prev->Next->Next = NULL;
            return;
        }
    }
    temp = CreateNewZip(item,NULL,item->GetZip());
}

int VotersFromZip(int Zip){
    ZipNode * temp = Head;
    while (temp != NULL)
    {
        if(temp->Zip == Zip){
            ItemNode * temp2 = temp->Items;
            std::cout << "For the Zip code " << Zip << " there are " << temp->NumberOfItems << " voters :\n";
            while(temp2 != NULL){
                temp2->item->DisplayVoter();
                temp2 = temp2->Next;
            }
            return 0;
        }
    }
    std::cout << "There are no voters for the zip code " << Zip << ".\n";
    return ERROR;
}

void DisplayZips(void){
    if (Head == NULL){
        std::cout << "No one has voted yet!\n";
        return;
    }
    int max,prevmax;
    ZipNode * temp ;
    
    max = Head->NumberOfItems;
    prevmax = 0;
    for (int i = 0 ; i < TotalZips;){
        temp = Head;
        while (temp != NULL)
        {      
            if (temp->NumberOfItems > max && (i==0 || temp->NumberOfItems < prevmax))
                max = temp->NumberOfItems;
            temp = temp->Next;
        }
        temp = Head;
        while( temp != NULL){
            if (temp->NumberOfItems == max){
                i++;
                std ::cout << "Zip : " << temp->Zip << " Have voted : " << temp->HaveVoted;
            }
            temp = temp->Next;
        }
        prevmax = max;
        max = 0;
    }
    
}


void AddVote(int Zip){
    ZipNode * temp = Head;
    while (temp!= NULL)
    {
        if(temp->Zip == Zip){
            temp->HaveVoted++;
            return;
        }
    }
    
}

void ExitList(void){
    ZipNode * temp = Head,*ToDelete;
    while (temp != NULL)
    {
        ItemNode * temp2 = temp->Items,*ToDelete2;
        while (temp2 != NULL)
        {
            ToDelete2 = temp2;
            temp2 = temp2->Next;
            delete ToDelete;
        }
        ToDelete = temp;
        temp = temp->Next;
    }
}