#include <iostream>
#include <fstream>
#include "DataLoader.h"
#include <cstring>
#include <tuple>
#include <vector>

using namespace std;

int main(){
    cout << "-------------Reading Database file-------------"<<"\n"<<"\n";

    DatabaseStorage storage{100000000,400};
    vector<tuple<string*, uint>> database;
    DataLoader loader{"games.txt"};

    for(int i=0;i<loader.getRecordSize();i++){
        tuple<string*,uint> recordAddress = storage.writeRecord(sizeof(loader.getRecordOnIndex(i)));
        database.push_back(recordAddress);
    }

    cout << "-------------Done Reading Database file-------------"<<"\n"<<"\n";

    cout << "-------------Information on Database-------------"<<"\n"<<"\n";
    cout << "Size of storage: " << storage.getSizeOfStorage() << "\n";
    cout << "Size of record: " << sizeof(loader.getRecordOnIndex(0)) << "\n";
    cout << "Number of records: " << loader.getRecordSize() << "\n";
    cout << "Number of records per block: " << storage.getBlockSize() / sizeof(loader.getRecordOnIndex(0)) << "\n";
    cout << "Number of blocks used for storing data: " << storage.getBlocksOccupied() << "\n";

    return 0;
}