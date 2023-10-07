#include <iostream>
#include <fstream>
#include <ctime>
#include <tuple>
#include <vector>
#include <algorithm>
#include <chrono>
#include "DataLoader.h"
#include "tree.h"

using namespace std;

int countDataBlocks(vector<Record*>records, vector<tuple<string*, uint>> database){
    vector<string*> addresses;
    int count = 0;
    for(int i=0;i<records.size();i++){
        for (int j=0;j<database.size();j++){
            tuple<string*,uint>individualRecord = database[j];
            string* blockAddress = get<0>(individualRecord);
            uint offset = get<1>(individualRecord);
            Record* retrievedRecord = reinterpret_cast<Record*>(blockAddress + offset);
            if(retrievedRecord == records[i]){
                if(find(addresses.begin(), addresses.end(), blockAddress) == addresses.end()){
                    addresses.push_back(blockAddress);
                    count ++;
                }
            }
        }
    }
    return count;
}

int main(){
    cout << "=====================================================" << endl;
    cout << "-------------Experiment 1-------------"<<"\n";
    cout << "-------------Reading Database file-------------"<<"\n"<<"\n";

    DatabaseStorage *storage = new DatabaseStorage(100000000,400);
    vector<tuple<string*, uint>> database;
    DataLoader *loader = new DataLoader("games.txt");
    BPlusTree bptree;

    for(int i=0;i<loader->getRecordSize();i++){
        int recordSize = sizeof(loader->getRecordOnIndex(i));
        tuple<string*,uint> recordAddress = storage->writeRecord(recordSize);

        // create copy of record
        Record recordCopy = loader->getRecordOnIndex(i);
        // full address of record using block location and offset in the block
        string* fullAddressOfRecord = get<0>(recordAddress) + get<1>(recordAddress);
        //Copy record data to allocated memory
        memcpy(fullAddressOfRecord, &recordCopy, recordSize);

        //Store the address of the record in a database vector
        database.push_back(recordAddress);
    }

    cout << "-------------Done Reading Database file-------------"<<"\n"<<"\n";
    int sizeOfRecord = sizeof(loader->getRecordOnIndex(0));
    cout << "-------------Information on Database-------------"<<"\n"<<"\n";
    cout << "Size of storage: " << storage->getSizeOfStorage() << "\n";
    cout << "Size of record: " << sizeOfRecord << "\n";
    cout << "Number of records: " << loader->getRecordSize() << "\n";
    cout << "Number of records per block: " << storage->getBlockSize() / sizeOfRecord << "\n";
    cout << "Number of blocks used for storing data: " << storage->getBlocksOccupied() << "\n";
    cout << "=====================================================" <<"\n"<<"\n";

    cout << "-------------Experiment 2-------------"<<"\n";
    cout << "-------------Indexing records onto B+ tree-------------"<<"\n";
    keys_struct *keys = new keys_struct[database.size()];

    // Insert into B+ tree
    for (unsigned int i=0;i<1000;i++){
        tuple<string*,uint>AddressOfRecord = database[i];
        // Reverse Engineer to retrieve the record and insert into B+ plus tree
        string* blockAddress = get<0>(AddressOfRecord);
        uint offset = get<1>(AddressOfRecord);
        Record* retrievedRecord = reinterpret_cast<Record*>(blockAddress + offset);
        bptree.insert(retrievedRecord);
    }

    cout << "-------------Done inserting into B+ tree-------------"<<"\n"<<"\n";
    cout << "=====================================================" << endl;
    cout << "-------------Experiment 3-----------------"<<"\n";
    auto start = chrono::high_resolution_clock::now();
    Node * node = bptree.search(0.5);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "Time taken to search for key 0.5: " << duration.count() << " microseconds" << "\n";
    if(node != nullptr){
        keys_struct key = node->getSpecificKey(0.5);
        vector<Record*> records = *(key.secondary_key);
        
        float total = 0.0;
        for(int i=0;i<records.size();i++){
            total += records[i]->fg3_pct_home;
        }
        float average = total/records.size();
        int dataBlocksAccessed = countDataBlocks(records, database);
        cout << "Total fg3_pct_home: " << total << "\n";
        cout << "All records with values 0.5 for fg_pct_home: " << records.size() << "\n";
        cout << "Average fg3_pct_home: " << average << "\n";
        cout << "Number of data blocks accessed: " << dataBlocksAccessed << "\n";
    }
    bptree.displayTree(bptree.getRoot(),true);

    cout << "=====================================================" << endl;
    cout << "-------------Experiment 4-----------------"<<"\n";
    vector<keys_struct> keysToBeDeleted;
    bptree.removeRange(0,0.35);
    cout << "=====================================================" << endl;
    bptree.displayTree(bptree.getRoot(),true);
    return 0;
}