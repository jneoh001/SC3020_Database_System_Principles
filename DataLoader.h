#ifndef DATALOADER_H
#define DATALOADER_H

#include <string>
#include <iostream>
#include <vector>
#include <tuple>
#include <sstream>

using namespace std;

typedef unsigned int uint;
typedef unsigned short int usint;

struct Record {
    string game_date;
    uint team_id;
    usint pts_home;
    float fg_pct_home;
    float ft_pct_home;
    float fg3_pct_home;
    usint ast_home;
    usint reb_home;
    bool home_win;
};

class DataLoader {
    private:
        string file_name;
        vector<Record> records;
        void load_data();
    public:
        DataLoader(string file_name);
        int getRecordSize(){
            return this->records.size();
        }
        Record getRecordOnIndex(int index){
            return this->records[index];
        }
};

DataLoader::DataLoader(string file_name){
    this->file_name = file_name;
    this->load_data();
}

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
    file.close();
}

class DatabaseStorage{
    private:
        string *storagePointer; // starting initial pointer for database storage
        string * curBlockPointer; // current pointer for block in database
        uint offset; // location of record in block
        uint storageCapacity;
        uint blockSize;
        uint blocksOccupied;
        uint blocksLeft;
    public:
        DatabaseStorage(uint storageCapacity, uint blockSize);
        ~DatabaseStorage();
        
        bool blockAvailable();
        tuple<string*, uint> writeRecord(uint recordSize);
        uint getBlockSize(){
            return blockSize;
        }
        uint getBlocksOccupied(){
            return blocksOccupied;
        }
        uint getBlocksLeft(){
            return blocksLeft;
        }
        uint getSizeOfStorage(){
            return storageCapacity;
        }
};

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

#endif