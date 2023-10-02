#ifndef DATALOADER_H
#define DATALOADER_H

#include <string>
#include <iostream>
#include <vector>

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
};


#endif