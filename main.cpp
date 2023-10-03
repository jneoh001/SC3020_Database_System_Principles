#include <iostream>
#include <fstream>
#include <cstring>
#include <tuple>
#include <vector>
#include "DataLoader.h"
#include "tree.h"

using namespace std;


int main(){
    cout << "-------------Reading Database file-------------"<<"\n"<<"\n";

    DatabaseStorage storage(100000000,400);
    vector<tuple<string*, uint>> database;
    DataLoader loader("games.txt");
    BPlusTree bptree;

    for(int i=0;i<loader.getRecordSize();i++){
        int recordSize = sizeof(loader.getRecordOnIndex(i));
        tuple<string*,uint> recordAddress = storage.writeRecord(recordSize);

        // create copy of record
        Record recordCopy = loader.getRecordOnIndex(i);
        // full address of record using block location and offset in the block
        string* fullAddressOfRecord = get<0>(recordAddress) + get<1>(recordAddress);
        //Copy record data to allocated memory
        memcpy(fullAddressOfRecord, &recordCopy, recordSize);

        //Store the address of the record in a database vector
        database.push_back(recordAddress);
    }

    cout << "-------------Done Reading Database file-------------"<<"\n"<<"\n";

    cout << "-------------Information on Database-------------"<<"\n"<<"\n";
    cout << "Size of storage: " << storage.getSizeOfStorage() << "\n";
    cout << "Size of record: " << sizeof(loader.getRecordOnIndex(0)) << "\n";
    cout << "Number of records: " << loader.getRecordSize() << "\n";
    cout << "Number of records per block: " << storage.getBlockSize() / sizeof(loader.getRecordOnIndex(0)) << "\n";
    cout << "Number of blocks used for storing data: " << storage.getBlocksOccupied() << "\n";

    cout << "-------------Indexing records onto B+ tree-------------"<<"\n"<<"\n";
    keys_struct *keys = new keys_struct[20];

    for (int i=0;i<10;i++){
        tuple<string*,uint>AddressOfRecord = database[i];
        // Reverse Engineer to retrieve the record and insert into B+ plus tree
        string* blockAddress = get<0>(AddressOfRecord);
        uint offset = get<1>(AddressOfRecord);
        Record* retrievedRecord = reinterpret_cast<Record*>(blockAddress + offset);
        keys[i].key_value = retrievedRecord->fg_pct_home;
        bptree.insert(keys[i]);
    }
    // Create dummy data
    // for (int i = 0; i < 20; i++)
    // {
    //     keys[i].key_value = i;
    //     bptree.insert(keys[i]);
    // }


    cout << "-------------Done inserting into B+ tree-------------"<<"\n"<<"\n";

    //Display B+ tree (DEBUGGING)
    bptree.displayTree(bptree.getRoot(),true);
    return 0;
}