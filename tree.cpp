#include <iostream>
#include <vector>
using namespace std;

struct keys_struct{
    float key_value;
    // vector <void*> add_vect;
};

// Define the B+ tree node structure
class Node {
    bool isLeaf;
    int size;
    Node ** ptr;
public:

    Node()
    {
        isLeaf = true;
    }

};

class InternalNode : public Node {
public:
    
};

class LeafNode : public Node {
public:
    vector <unsigned int> TEAM_ID_home;
    vector <unsigned short> PTS_home;
    vector <float> FG_PCT_home;
    vector <float> FT_PCT_home;
    vector <float> FG3_PCT_home;
    vector <unsigned short> AST_home;
    vector <unsigned short> REB_home;
    vector <bool> HOME_TEAM_WINS;
};

//GAME_DATE_EST	TEAM_ID_home	PTS_home	FG_PCT_home	FT_PCT_home	FG3_PCT_home	AST_home	REB_home	HOME_TEAM_WINS
//  22/12/2022	1610612740	126	0.484	0.926	0.382	25	46	1

// Define the B+ tree class
class BPlusTree {

    Node* root;


public:
    BPlusTree() {
        // Initialize the B+ tree
        root = NULL;
    }


    void insert( keys_struct nodeData) {

    }

    int search(int key) {

        

        return -1; 
    }

    void remove(int key) {
    }

    void print(){

    }






int main() {
    BPlusTree bptree;

    bptree.insert(10, 100);
    bptree.insert(20, 200);
    bptree.insert(5, 50);

    // Test out to find 

    int result = bptree.search(20);
    if (result != -1) {
        std::cout << "Key 20 found with value: " << result << std::endl;
    } else {
        std::cout << "Key 20 not found." << std::endl;
    }

    bptree.print();

    return 0;
}





