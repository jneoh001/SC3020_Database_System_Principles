#include <iostream>
#include <fstream>
#include "DataLoader.h"
#include <cstring>
#include <tuple>
#include <vector>

using namespace std;


int main(){
    cout << "-------------Reading Database file-------------"<<"\n"<<"\n";

    DatabaseStorage storage(100000000,400);
    vector<tuple<string*, uint>> database;
    DataLoader loader("games.txt");

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

        // Reverse Engineer to retrieve the record
        string* blockAddress = get<0>(recordAddress);
        uint offset = get<1>(recordAddress);
        Record* retrievedRecord = reinterpret_cast<Record*>(blockAddress + offset);
        cout << "Details of record: " << retrievedRecord->fg3_pct_home << endl;
    }

    cout << "-------------Done Reading Database file-------------"<<"\n"<<"\n";

    cout << "-------------Information on Database-------------"<<"\n"<<"\n";
    cout << "Size of storage: " << storage.getSizeOfStorage() << "\n";
    cout << "Size of record: " << sizeof(loader.getRecordOnIndex(0)) << "\n";
    cout << "Number of records: " << loader.getRecordSize() << "\n";
    cout << "Number of records per block: " << storage.getBlockSize() / sizeof(loader.getRecordOnIndex(0)) << "\n";
    cout << "Number of blocks used for storing data: " << storage.getBlocksOccupied() << "\n";

    cout << "-------------Indexing records onto B+ tree-------------"<<"\n"<<"\n";

    return 0;
}