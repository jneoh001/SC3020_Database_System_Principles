#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
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
            cout << "Ignored row" << endl;
            continue;  // Skip records with conversion errors
        }
    }
}


int main(){
    DataLoader loader = DataLoader("games.txt");
    cout << "Data loaded successfully!" << endl;
    return 0;
}