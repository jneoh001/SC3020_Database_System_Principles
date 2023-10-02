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
    while(getline(file, line)){
        Record record;
        stringstream ss(line);
        string attribute;
        while (getline(ss, attribute, '\t')){
            
        }
        
    }
}

int main(){
    DataLoader DataLoader("games.txt");
}