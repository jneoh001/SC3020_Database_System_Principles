#include <iostream>
#include <vector>
using namespace std;

const int MAX_KEYS_NODE = 4;

struct keys_struct
{
    float key_value;
    vector<void *> secondary_key;
};

// Define the B+ tree node structure
class Node
{
    bool isLeaf;
    int size;
    keys_struct *key;
    Node **ptr;
    friend class BPlusTree;

public:
    // constructor
    Node()
    {
        key = new keys_struct[MAX_KEYS_NODE];
        ptr = new Node *[MAX_KEYS_NODE + 1];
        isLeaf = true;
    }
};

class InternalNode : public Node
{
public:
};

class LeafNode : public Node
{
public:
    vector<unsigned int> TEAM_ID_home;
    vector<unsigned short> PTS_home;
    vector<float> FG_PCT_home;
    vector<float> FT_PCT_home;
    vector<float> FG3_PCT_home;
    vector<unsigned short> AST_home;
    vector<unsigned short> REB_home;
    vector<bool> HOME_TEAM_WINS;
};

// GAME_DATE_EST	TEAM_ID_home	PTS_home	FG_PCT_home	FT_PCT_home	FG3_PCT_home	AST_home	REB_home	HOME_TEAM_WINS
//   22/12/2022	1610612740	126	0.484	0.926	0.382	25	46	1

// Define the B+ tree class
class BPlusTree
{

    Node *root;

    void insertInternal(keys_struct nodeData, Node *current, Node *child)
    {

        // 2 Scenarios : Still have space to add key in node OR Dont have space.

        // Scenario 1. Still have space available
        if (current->size < MAX_KEYS_NODE)
        {

            // Determine new key's position in node.
            int i = 0;
            while (nodeData.key_value > current->key[i].key_value && i < current->size)
            {
                i = i + 1;
            }

            // Using this new position, we will push back all keys and pointers by 1 space.
            // E.g if we want to insert 4 :  [2][3][5][] becomes [2][3][4][5]

            // Keys
            for (int j = current->size; j > i; j--)
            {
                current->key[j] = current->key[j - 1]; // We start from the back and iterate,
            }

            // Pointers
            for (int j = current->size + 1; j > i; j--)
            {
                current->ptr[j] = current->ptr[j - 1];
            }

            // Finally we add in the key and pointer.
            //  We also increase/update node size count.
            current->size++;
            current->key[i] = nodeData;
            current->ptr[i + 1] = child;
        }

        // Scenario 2
        //  If there is not enough space, we will have to split the keys into two nodes
        Node *newInternalNode = new Node; // Create another node

        // Now we want to create a temp holding node

        Node *tempPtr[MAX_KEYS_NODE + 2];
        keys_struct tempKey[MAX_KEYS_NODE + 1];

        // Copy keys and pointers of existing node
        for (int i = 0; i < MAX_KEYS_NODE; i++)
            tempKey[i] = current->key[i];
        for (int i = 0; i < MAX_KEYS_NODE + 1; i++)
            tempPtr[i] = current->ptr[i];

        int i = 0;

        // Similar to above Scenario 1 ; we will determine new keys position and shift.

        while (nodeData.key_value > tempKey[i].key_value && i < MAX_KEYS_NODE)
            i++;

        for (int j = MAX_KEYS_NODE + 1; j > i; j--)
            tempKey[j] = tempKey[j - 1];
        for (int j = MAX_KEYS_NODE + 2; j > i; j--)
            tempPtr[j] = tempPtr[j - 1];

        tempPtr[i + 1] = child;
        newInternalNode->isLeaf = false;
        current->size = (MAX_KEYS_NODE + 1) / 2;
        newInternalNode->size = MAX_KEYS_NODE - ((MAX_KEYS_NODE + 1) / 2);
        
        int j = 0;

        for (i = 0, j = current->size + 1; i < newInternalNode->size; i++, j++)
            newInternalNode->key[i] = tempKey[j];

        for (i = 0, j = current->size + 1; i < newInternalNode->size + 1; i++, j++)
            newInternalNode->ptr[i] = tempPtr[j];

        if (current == root)
        {
            Node *newRootNode = new Node;
            newRootNode->key[0] = current->key[current->size];
            newRootNode->ptr[0] = current;
            newRootNode->ptr[1] = newInternalNode;
            newRootNode->isLeaf = false;
            newRootNode->size = 1;
            root = newRootNode;
        }
        else
        {
            insertInternal(current->key[current->size], findParent(root, current), newInternalNode);
        }
    }

public:
    BPlusTree()
    {
        // Initialize the B+ tree
        root = NULL;
    }

    int search(int key)
    {

        return -1;
    }

    void remove(int key)
    {
    }

    void print()
    {
    }
};

int main()
{
    BPlusTree bptree;

    // bptree.insert(10, 100);
    //  bptree.insert(20, 200);
    //  bptree.insert(5, 50);

    // Test out to find

    int result = bptree.search(20);
    if (result != -1)
    {
        std::cout << "Key 20 found with value: " << result << std::endl;
    }
    else
    {
        std::cout << "Key 20 not found." << std::endl;
    }

    bptree.print();

    return 0;
}