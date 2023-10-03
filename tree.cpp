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
    Node(int maxsize = MAX_KEYS_NODE)
    {
        key = new keys_struct[maxsize];
        ptr = new Node*[maxsize+1];
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

    void insertKeyIntoInternalNode(keys_struct entry, Node* cursor, Node* child){
        //function to insert extra key into an internal node and point to a new child
        //Scenario 1: Node is not full
        //logic is same as insert
        if(cursor->size<MAX_KEYS_NODE){
            int i = 0, j;
            while(entry.key_value>cursor->key[i].key_value && i<cursor->size)i++;
            for(j=cursor->size;j>i;j--){
                cursor->key[j] = cursor->key[j-1];
                //move both left and right pointers
                cursor->key[j+1] = cursor->key[j];
            }
            //ptr to child should be right pointer, key should be min of child
            cursor->key[i] = entry;
            cursor->size++;
            cursor->ptr[i+1] = child;
        }
        else{
            Node* newInternal = new Node;
            newInternal->isLeaf = false;
            Node* virtualNode = new Node(MAX_KEYS_NODE+1);
            virtualNode->isLeaf = false;
            for(int i=0;i<MAX_KEYS_NODE;i++){
                virtualNode->key[i] = cursor->key[i];
                virtualNode->ptr[i] = cursor->ptr[i];
            }
            //rightmost pointer
            virtualNode->ptr[MAX_KEYS_NODE] = cursor->ptr[MAX_KEYS_NODE];
            int i = 0, j;
            while (virtualNode->key[i].key_value<entry.key_value && i<MAX_KEYS_NODE)i++;
            for(j=MAX_KEYS_NODE+1;j>i;j--){
                virtualNode->key[j] = virtualNode->key[j-1];
                //move only right pointers
                virtualNode->ptr[j+1] = virtualNode->ptr[j];
            }
            virtualNode->key[i] = entry;
            virtualNode->ptr[i+1] = child;
            cursor->size = (MAX_KEYS_NODE+1)/2;
            newInternal->size = MAX_KEYS_NODE - cursor->size;
            //move both elements and pointers to new node
            //key[cursorsize] is moved up a level
            for(i=0,j=cursor->size+1;i<newInternal->size;i++,j++){
                newInternal->key[i] = virtualNode->key[j];
                newInternal->ptr[i] = virtualNode->ptr[j];
            }
            //rightmost pointer
            newInternal->ptr[newInternal->size] = virtualNode->ptr[MAX_KEYS_NODE+2];
            if(cursor==root){
                rootSplitter(cursor, newInternal, cursor->key[cursor->size]);
            }
            else{
                //recur
            }
            nodeCleaner(cursor);
        }
    }

    //creates a new root node when the old root node overflows
    void* rootSplitter(Node* oldRoot, Node* newChild, keys_struct newkey){
        Node *newRoot = new Node;
        newRoot->key[0] = newkey;
        newRoot->ptr[0] = oldRoot;
        newRoot->ptr[1] = newChild;
        newRoot->isLeaf = false;
        newRoot->size = 1;
        root = newRoot;
    }

    //removes all keys and ptrs in a node after index nodesize
    void* nodeCleaner(Node* cursor){
        keys_struct cleaner;
        for (int i=cursor->size;i<MAX_KEYS_NODE;i++){
            cursor->key[i] = cleaner;
            cursor->ptr[i] = nullptr;
        }
    }

    //
    Node* findParent(Node* child){
        
    }

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
  
    Node* search(float key) {
        
        return nullptr; 
    }
  
    void insert(keys_struct entry){
        //Tree is empty
        if (root==NULL){
            root = new Node;
            root->key[0] = entry;
            root->isLeaf = true;
            root->size = 1;
            return;
        }
        //Tree is not empty
        {
            Node* cursor = root;
            Node* parent;

            //Check if key in tree
            Node* searcher;
            searcher = search(entry.key_value);
            if (searcher != nullptr){
                for(int i=0;i<searcher->size;i++){
                    if(searcher->key[i].key_value == entry.key_value){
                        //push entire secondary key without checking like FAQ 9
                        searcher->key[i].secondary_key.push_back(entry.secondary_key[0]);
                        break;
                    }
                }
                return;
            }

            //Search for node to insert new key
            while(cursor->isLeaf != true){
                parent = cursor;
                for(int i=0;i<cursor->size;i++){
                    //Search for key > entry key, go left ptr of key
                    if(entry.key_value<cursor->key[i].key_value){
                        cursor = cursor->ptr[i];
                        break;
                    }
                    //If entry key is greater than all keys in node
                    if(i==cursor->size-1){
                        cursor=cursor->ptr[i+1];
                        break;
                    }
                }
            }

            //Attempt to insert the new key
            //If node is not full
            if(cursor->size<MAX_KEYS_NODE){
                int i = 0;
                while(cursor->key[i].key_value<entry.key_value && i<cursor->size) i++;
                for(int j=cursor->size;j>i;j--){
                    cursor->key[j] = cursor->key[j-1];
                    //If node is not full, right ptr of leaf node will be null, except ptr to next node
                    cursor->ptr[j] = cursor->ptr[j-1];
                }
                cursor->key[i] = entry;
                cursor->size++;
                //cursor->ptr[i] = entryptr;
                //need a ptr to record
            }
            else{
                //Node is full
                //Create a new leaf node
                Node* newLeaf = new Node;
                //Create a virtual node size MAX_KEYS_NODE + 1
                keys_struct virtualNode[MAX_KEYS_NODE+1];
                for (int i=0;i<MAX_KEYS_NODE;i++){
                    virtualNode[i] = cursor->key[i];
                }
                int i = 0, j;
                //Search for position to insert entry, maximum is at MAX+1
                while (virtualNode[i].key_value<entry.key_value && i<MAX_KEYS_NODE)i++;
                for(int j=MAX_KEYS_NODE+1;j>i;j--){
                    virtualNode[j] = virtualNode[j-1];
                }
                virtualNode[i] = entry;
                //default isLeaf = true but just in case
                newLeaf->isLeaf = true;
                //Split the cursor node
                cursor->size = (MAX_KEYS_NODE+1)/2;
                newLeaf->size = (MAX_KEYS_NODE+1) - cursor->size;
                //temp node for later
                Node* temp = cursor->ptr[MAX_KEYS_NODE];
                //place keys into leafnodes, until new key
                //Scenario 1: new key is in the left node
                if(i<cursor->size){
                    //move all keys and ptr into larger node first
                    for(int j=0,k=cursor->size;j<newLeaf->size;j++,k++){
                        newLeaf->key[j] = virtualNode[k];
                        newLeaf->ptr[j] = cursor->ptr[k-1];
                        //All keys to the right of i are shifted right by 1
                        //k has no termination condition, it should never = MAX+1
                    }
                    //move all keys and ptr to the right of new key
                    for(int j=cursor->size-1;j>i;j--){
                        cursor->key[j] = cursor->key[j-1];
                        cursor->ptr[j] = cursor->ptr[j-1];
                    }
                    //insert new key and ptr at i
                    cursor->key[i] = entry;
                    //cursor->ptr[i] = entryptr
                }else{
                    //Scenario 2: new key is in the right node
                    //move all keys and ptr to the left of i into the right node
                    int j = cursor->size, k=0;
                    while(j<i,k<newLeaf->size){
                        newLeaf->key[k] = cursor->key[j];
                        newLeaf->ptr[k] = cursor->ptr[j];
                        k++;j++;
                    }
                    //insert newkey and newptr
                    newLeaf->key[k] = entry;
                    //newLeaf->ptr[k] = entryptr;
                    k++;
                    //insert the rest of the keys and ptrs
                    while(j<MAX_KEYS_NODE,k<newLeaf->size){
                        newLeaf->key[k] = cursor->key[j];
                        newLeaf->ptr[k] = cursor->ptr[j];
                        k++;j++;
                    }
                }
                //clean left node after cursorsize
                nodeCleaner(cursor);
                //New leafnode points to next node
                //Cursor points to new leafnode
                newLeaf->ptr[MAX_KEYS_NODE] = temp;
                cursor->ptr[MAX_KEYS_NODE] = newLeaf;
                //modify the parent
                //Scenario 1: cursor is root node
                if(cursor == root){
                    //Create a new root
                    rootSplitter(cursor, newLeaf, newLeaf->key[0]);
                }else{
                    //Scenario 2: Cursor is not a root node
                    //Insert new key in parent node
                    insertKeyIntoInternalNode(newLeaf->key[0], parent, newLeaf);
                }
            }
        }
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

    Node* result = bptree.search(20);
    if (result != nullptr) {
        std::cout << "Key 20 found with value: " << result << std::endl;
    }
    else
    {
        std::cout << "Key 20 not found." << std::endl;
    }

    bptree.print();

    return 0;
}