#include <iostream>
#include <fstream>
#include <ctime>
#include <tuple>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
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

int countDataBlocksBruteForce(vector<Record*>records, vector<tuple<string*, uint>> database){
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
                    break;
                }
            }
            count ++;
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
    for (unsigned int i=0;i<database.size();i++){
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

        auto start1 = chrono::high_resolution_clock::now();
        int bruteForce = countDataBlocksBruteForce(records, database);
        auto stop1 = chrono::high_resolution_clock::now();
        auto duration1 = chrono::duration_cast<chrono::microseconds>(stop1 - start1);
        cout << "Total fg3_pct_home: " << total << "\n";
        cout << "All records with values 0.5 for fg_pct_home: " << records.size() << "\n";
        cout << "Average fg3_pct_home: " << average << "\n";
        cout << "Number of data blocks accessed: " << dataBlocksAccessed << "\n";
        cout << "Number of data blocks accessed via brute force linear scan: "<< bruteForce << "\n";
        cout << "Time taken for brute force liear scan: " << duration1.count() << " microseconds" << "\n";
    }

    cout << "\n\n=====================================================" << endl;
    cout << "-------------Experiment 4-----------------"<<"\n";
    auto start2 = chrono::high_resolution_clock::now();
    vector<keys_struct> searchrange = bptree.searchRange(0.6, 1.00);
    auto stop2 = chrono::high_resolution_clock::now();
    auto duration2 = chrono::duration_cast<chrono::microseconds>(stop2 - start2);
    cout << "Time taken to search for key range 0.6 to 1: " << duration2.count() << " microseconds" << "\n";
    float total = 0.0;
    int totalsize = 0;
    int totaldataBlocksAccessed = 0;
    int totaldataBlocksAccessedBruteForce = 0;
    for(int i= 0;i<searchrange.size();i++){
        vector<Record*> records = *(searchrange[i].secondary_key);
        totalsize += records.size();
        for(int i=0;i<records.size();i++){
            total += records[i]->fg3_pct_home;
        }
        totaldataBlocksAccessed += countDataBlocks(records, database);
    }
    float average = total/totalsize;
    auto start3 = chrono::high_resolution_clock::now();
    for(int i=0;i<searchrange.size();i++){
        vector<Record*> records = *(searchrange[i].secondary_key);
        totaldataBlocksAccessedBruteForce += countDataBlocksBruteForce(records, database);
    }
    auto stop3 = chrono::high_resolution_clock::now();
    auto duration3 = chrono::duration_cast<chrono::microseconds>(stop3 - start3);
    cout << "Total fg3_pct_home: " << total << "\n";
    cout << "All records with values 0.6 to 1 for fg_pct_home: " << totalsize << "\n";
    cout << "Average fg3_pct_home: " << average << "\n";
    cout << "Number of data blocks accessed: " << totaldataBlocksAccessed << "\n";
    cout << "Number of data blocks accessed via brute force linear scan: "<< totaldataBlocksAccessedBruteForce << "\n";
    cout << "Time taken for brute force liear scan: " << duration3.count() << " microseconds" << "\n";
    cout << "\n\n=====================================================" << endl;
    cout << "-------------Experiment 5-----------------"<<"\n";
    vector<keys_struct> keysToBeDeleted;
    int totaldataBlocksAccessedBruteForce2 = 0;
    auto start4 = chrono::high_resolution_clock::now();
    keysToBeDeleted = bptree.searchRange(0.0,0.35);
    for(int i=0;i<keysToBeDeleted.size();i++){
        vector<Record*> records = *(keysToBeDeleted[i].secondary_key);
        totaldataBlocksAccessedBruteForce2 += countDataBlocksBruteForce(records, database);
    }
    auto stop4 = chrono::high_resolution_clock::now();
    auto duration4 = chrono::duration_cast<chrono::microseconds>(stop4 - start4);
    cout << "Number of data blocks accessed via brute force linear scan: "<< totaldataBlocksAccessedBruteForce2 << "\n";
    auto start5 = chrono::high_resolution_clock::now();
    keysToBeDeleted = bptree.searchRange(0.0,0.35);
    for(int i=0;i<keysToBeDeleted.size();i++){
        bptree.remove(keysToBeDeleted[i]);
    }
    auto stop5 = chrono::high_resolution_clock::now();
    auto duration5 = chrono::duration_cast<chrono::microseconds>(stop5 - start5);
    cout << "Time taken to search for key range 0 to 0.35 and delete them: " << duration5.count() << " microseconds" << "\n";
    bptree.displayTree(bptree.getRoot(),true);
    cout << "=====================================================" << endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100000));
    return 0;
}
