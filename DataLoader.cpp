#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include "DataLoader.h"

using namespace std;

DataLoader::DataLoader(string file_name){
    this->file_name = file_name;
    this->load_data();
};

void DataLoader::load_data(){
    ifstream file(this->file_name);
    string line;
    while(getline(file, line, '\n')){
        vector <string> record;
        stringstream ss(line);
        string attribute;
        while (getline(ss, attribute, '\t')){
            record.push_back(attribute);
        }
        try {
            Record new_record = {
                record[0],
                static_cast<unsigned int>(stoi(record[1])),
                static_cast<unsigned short int>(stoi(record[2])),
                stof(record[3]),
                stof(record[4]),
                stof(record[5]),
                static_cast<unsigned short int>(stoi(record[6])),
                static_cast<unsigned short int>(stoi(record[7])),
                static_cast<bool>(stoi(record[8]))
            };
            this->records.push_back(new_record);
        } catch (const std::exception& e) {
            continue;  // Skip records with conversion errors
        }
    }
}

DatabaseStorage::DatabaseStorage(uint storageCapacity, uint blockSize){
    this->storageCapacity = storageCapacity;
    this->blockSize = blockSize;
    this->blocksOccupied = 0;
    this->blocksLeft = storageCapacity / blockSize;
    this->storagePointer = new string[storageCapacity];
    this->curBlockPointer = storagePointer;
    this->offset = 0;
}

DatabaseStorage::~DatabaseStorage(){
    delete[] this->storagePointer;
    storagePointer = nullptr;
}

bool DatabaseStorage::blockAvailable(){
    if (blocksLeft > 0){
        curBlockPointer += blockSize; //allocate a block
        blocksLeft -= 1;
        blocksOccupied += 1;
        offset = 0;
        return true;
    }else{
        cout << "No more space available in storage" <<endl;
        return false;
    }
}
tuple<string*, uint> DatabaseStorage::writeRecord(uint recordSize){
    if (blockSize < offset + recordSize){
        bool blockAllocated = blockAvailable();
        if (!blockAllocated){
            cerr << "Unable to write record as no space available for record in the block" <<endl;
        }
    }
    if (blockSize < recordSize){
        cerr << "Unable to write record to block where record size greater than block size" << endl;
    }

    tuple<string*, uint> recordAddress(curBlockPointer, offset);
    offset += recordSize;

    return recordAddress;
}


int main(){
    DataLoader loader = DataLoader("games.txt");
    cout << "Data loaded successfully!" << endl;
    return 0;
}