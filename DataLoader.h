#ifndef DATALOADER_H
#define DATALOADER_H

#include <string>
#include <iostream>
#include <vector>
#include <tuple>

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


#endif