#ifndef DataLoader
#define DataLoader

#include <string>
#include <iostream>
#include <vector>

using namespace std;

typedef unsigned int uint;
typedef unsigned short int usint;

struct Record {
    int game_data;
    uint team_id;
    usint pts_home;
    float fg_pct_home;
    float ft_pct_home;
    float fg3_pct_home;
    usint ast_home;
    usint reb_home;
    bool home_win;
};


#endif