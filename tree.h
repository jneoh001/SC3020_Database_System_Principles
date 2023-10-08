#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip>

using namespace std;

const int MAX_KEYS_NODE = 4;

struct keys_struct
{
    //float key_value;
    float key_value = 0.0;
    vector<Record *> *secondary_key = new vector<Record *>(); // create new vector object

    void reset()
    {
        key_value = NULL;
        secondary_key->clear();
    }
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
        key = new keys_struct[maxsize]; // n keys
        ptr = new Node *[maxsize + 1];  // n+1 pointers (store address to record + 1 to next node)
        isLeaf = true;
    }

    //getter functions
    keys_struct *getKey()
    {
        return key;
    }
    keys_struct getSpecificKey(float x)
    {
        for (int i = 0; i < size; i++)
        {
            if (key[i].key_value == x)
            {
                return key[i];
            }
        }
    }
    Node **getPtr()
    {
        return ptr;
    }
    bool getIsLeaf()
    {
        return isLeaf;
    }
    int getSize()
    {
        return size;
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

    void insertKeyIntoInternalNode(keys_struct entry, Node *cursor, Node *child)
    {
        // function to insert extra key into an internal node and point to a new child
        // Scenario 1: Node is not full
        // logic is same as insert
        if (cursor->size < MAX_KEYS_NODE)
        {
            int i = 0, j;
            while (entry.key_value > cursor->key[i].key_value && i < cursor->size)
                i++;
            for (j = cursor->size; j > i; j--)
            {
                cursor->key[j] = cursor->key[j - 1];
                // move both left and right pointers
                cursor->key[j + 1] = cursor->key[j];
            }
            // ptr to child should be right pointer, key should be min of child
            cursor->key[i] = entry;
            cursor->size++;
            cursor->ptr[i + 1] = child;
        }
        else
        {
            Node *newInternal = new Node;
            newInternal->isLeaf = false;
            Node *virtualNode = new Node(MAX_KEYS_NODE + 1);
            virtualNode->isLeaf = false;
            for (int i = 0; i < MAX_KEYS_NODE; i++)
            {
                virtualNode->key[i] = cursor->key[i];
                virtualNode->ptr[i] = cursor->ptr[i];
            }
            // rightmost pointer
            virtualNode->ptr[MAX_KEYS_NODE] = cursor->ptr[MAX_KEYS_NODE];
            int i = 0, j;
            while (virtualNode->key[i].key_value < entry.key_value && i < MAX_KEYS_NODE)
                i++;
            for (j = MAX_KEYS_NODE + 1; j > i; j--)
            {
                virtualNode->key[j] = virtualNode->key[j - 1];
                // move only right pointers
                virtualNode->ptr[j + 1] = virtualNode->ptr[j];
            }
            virtualNode->key[i] = entry;
            virtualNode->ptr[i + 1] = child;
            cursor->size = (MAX_KEYS_NODE + 1) / 2;
            newInternal->size = MAX_KEYS_NODE - cursor->size;
            // move both elements and pointers to new node
            // key[cursorsize] is moved up a level
            for (i = 0, j = cursor->size + 1; i < newInternal->size; i++, j++)
            {
                newInternal->key[i] = virtualNode->key[j];
                newInternal->ptr[i] = virtualNode->ptr[j];
            }
            // rightmost pointer
            newInternal->ptr[newInternal->size] = virtualNode->ptr[MAX_KEYS_NODE + 2];
            if (cursor == root)
            {
                rootSplitter(cursor, newInternal, cursor->key[cursor->size]);
            }
            else
            {
                // recur
            }
            nodeCleaner(cursor);
        }
    }

    // creates a new root node when the old root node overflows
    void rootSplitter(Node *oldRoot, Node *newChild, keys_struct newkey)
    {
        Node *newRoot = new Node;
        newRoot->key[0] = newkey;
        newRoot->ptr[0] = oldRoot;
        newRoot->ptr[1] = newChild;
        newRoot->isLeaf = false;
        newRoot->size = 1;
        root = newRoot;
        // cout << "root splitted" << endl;
    }

    // removes all keys and ptrs in a node after index nodesize
    void nodeCleaner(Node *cursor)
    {
        // cout << "node is being cleaned" << endl;
        for (int i = cursor->size; i < MAX_KEYS_NODE; i++)
        {
            cursor->key[i].reset();
            cursor->ptr[i] = nullptr;
        }
    }

    Node *findParent(Node *cursor, Node *child)
    {
        Node *parent;
        if (cursor->isLeaf || (cursor->ptr[0])->isLeaf)
        {
            return NULL;
        }
        for (int i = 0; i < cursor->size + 1; i++)
        {
            if (cursor->ptr[i] == child)
            {
                parent = cursor;
                return parent;
            }
            else
            {
                parent = findParent(cursor->ptr[i], child);
                if (parent != NULL)
                    return parent;
            }
        }
        return parent;
    }

    void merge(Node *leftNode, Node *rightNode)
    {
        for (int i = leftNode->size, j = 0; j < rightNode->size; i++, j++)
        {
            leftNode->key[i] = rightNode->key[j];
            leftNode->ptr[i] = rightNode->ptr[j];
        }
        leftNode->ptr[MAX_KEYS_NODE] = rightNode->ptr[MAX_KEYS_NODE];
        leftNode->size += rightNode->size;
    }

    void insertInternal(keys_struct nodeData, Node *current, Node *child)
    {
        // cout << "insertInternal start running" << endl;
        //  2 Scenarios : Still have space to add key in node OR Dont have space.

        // Scenario 1. Still have space available
        if (current->size < MAX_KEYS_NODE)
        {
            // cout << "scenario 1" << endl;
            //  Determine new key's position in node.
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
        else
        {
            // cout << "scenario 2" << endl;
            //  Scenario 2
            //   If there is not enough space, we will have to split the keys into two nodes
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

            for (int j = MAX_KEYS_NODE; j > i; j--)
                tempKey[j] = tempKey[j - 1];
            for (int j = MAX_KEYS_NODE + 1; j > i; j--)
                tempPtr[j] = tempPtr[j - 1];

            tempKey[i] = nodeData;
            tempPtr[i + 1] = child;
            newInternalNode->isLeaf = false;
            current->size = (MAX_KEYS_NODE + 1) / 2;
            newInternalNode->size = MAX_KEYS_NODE - ((MAX_KEYS_NODE + 1) / 2);

            int j = 0;
            for (i = 0; i < current->size; i++)
                current->key[i] = tempKey[i];

            for (i = 0; i < current->size + 1; i++)
                current->ptr[i] = tempPtr[i];

            for (i = 0, j = current->size + 1; i < newInternalNode->size; i++, j++)
                newInternalNode->key[i] = tempKey[j];

            for (i = 0, j = current->size + 1; i < newInternalNode->size + 1; i++, j++)
                newInternalNode->ptr[i] = tempPtr[j];

            if (current == root)
            {
                Node *newRootNode = new Node;
                newRootNode->key[0] = tempKey[current->size];
                newRootNode->ptr[0] = current;
                newRootNode->ptr[1] = newInternalNode;
                newRootNode->isLeaf = false;
                newRootNode->size = 1;
                root = newRootNode;
            }
            else
            {
                insertInternal(tempKey[current->size], findParent(root, current), newInternalNode);
            }
        }
    }

    void removeInternal(keys_struct x, Node* cursor, Node* child)
    {
        //deleting the key x first
        //checking if key from root is to be deleted
        if(cursor == root)
        {
            if(cursor->size == 1)//if only one key is left, change root
            {
                if(cursor->ptr[1] == child)
                {
                    delete[] child->key;
                    delete[] child->ptr;
                    delete child;
                    cout << "Deleted 1" <<"\n";
                    root = cursor->ptr[0];
                    delete[] cursor->key;
                    delete[] cursor->ptr;
                    delete cursor;
                    cout << "Deleted 1" <<"\n";
                    //cout<<"Changed root node\n";
                    return;
                }
                else if(cursor->ptr[0] == child)
                {
                    delete[] child->key;
                    delete[] child->ptr;
                    delete child;
                    cout << "Deleted 1" <<"\n";
                    root = cursor->ptr[1];
                    delete[] cursor->key;
                    delete[] cursor->ptr;
                    delete cursor;
                    cout << "Deleted 1" <<"\n";
                    //cout<<"Changed root node\n";
                    return;
                }
            }
        }
        int pos;
        for(pos = 0; pos < cursor->size; pos++)
        {
            if(cursor->key[pos].key_value == x.key_value)
            {
                break;
            }
        }
        for(int i = pos; i < cursor->size; i++)
        {
            cursor->key[i] = cursor->key[i+1];
        }
        //now deleting the pointer child
        for(pos = 0; pos < cursor->size+1; pos++)
        {
            if(cursor->ptr[pos] == child)
            {
                break;
            }
        }
        for(int i = pos; i < cursor->size+1; i++)
        {
            cursor->ptr[i] = cursor->ptr[i+1];
        }
        cursor->size--;
        if(cursor->size >= (MAX_KEYS_NODE +1) / 2 - 1)//no underflow
        {
            cout<<"Deleted "<< x.key_value << " " <<" from internal node successfully\n";
            return;
        }
        //cout<<"Underflow in internal node!\n";
        //underflow, try to transfer first
        if(cursor==root)return;
        Node* parent = findParent(root, cursor);
        int leftSibling, rightSibling;
        //finding left n right sibling of cursor
        for(pos = 0; pos < parent->size+1; pos++)
        {
            if(parent->ptr[pos] == cursor)
            {
                leftSibling = pos - 1;
                rightSibling = pos + 1;
                break;
            }
        }
        //try to transfer
        if(leftSibling >= 0)//if left sibling exists
        {
            Node *leftNode = parent->ptr[leftSibling];
            //check if it is possible to transfer
            if(leftNode->size >= (MAX_KEYS_NODE+1)/2)
            {
                //make space for transfer of key
                for(int i = cursor->size; i > 0; i--)
                {
                    cursor->key[i] = cursor->key[i-1];
                }
                //transfer key from left sibling through parent
                cursor->key[0] = parent->key[leftSibling];
                parent->key[leftSibling] = leftNode->key[leftNode->size-1];
                //transfer last pointer from leftnode to cursor
                //make space for transfer of ptr
                for (int i = cursor->size+1; i > 0; i--)
                {
                    cursor->ptr[i] = cursor->ptr[i-1];
                }
                //transfer ptr
                cursor->ptr[0] = leftNode->ptr[leftNode->size];
                cursor->size++;
                leftNode->size--;
                //cout<<"Transferred "<<cursor->key[0].key_value<<" "<< x.add_vect[0] <<" from left sibling of internal node\n";
                return;
            }
        }
        if(rightSibling <= parent->size)//check if right sibling exist
        {
            Node *rightNode = parent->ptr[rightSibling];
            //check if it is possible to transfer
            if(rightNode->size >= (MAX_KEYS_NODE+1)/2)
            {
                //transfer key from right sibling through parent
                cursor->key[cursor->size] = parent->key[pos];
                parent->key[pos] = rightNode->key[0];
                for (int i = 0; i < rightNode->size -1; i++)
                {
                    rightNode->key[i] = rightNode->key[i+1];
                }
                //transfer first pointer from rightnode to cursor
                //transfer ptr
                cursor->ptr[cursor->size+1] = rightNode->ptr[0];
                for (int i = 0; i < rightNode->size; ++i)
                {
                    rightNode->ptr[i] = rightNode->ptr[i+1];
                }
                cursor->size++;
                rightNode->size--;
                //cout<<"Transferred "<<cursor->key[0].key_value << " " << x.add_vect[0] <<" from right sibling of internal node\n";
                return;
            }
        }
        //transfer wasnt posssible hence do merging
        if(leftSibling >= 0)
        {
            //leftnode + parent key + cursor
            Node *leftNode = parent->ptr[leftSibling];
            leftNode->key[leftNode->size] = parent->key[leftSibling];
            for(int i = leftNode->size+1, j = 0; j < cursor->size; j++)
            {
                leftNode->key[i] = cursor->key[j];
            }
            for(int i = leftNode->size+1, j = 0; j < cursor->size+1; j++)
            {
                leftNode->ptr[i] = cursor->ptr[j];
                cursor->ptr[j] = NULL;
            }
            leftNode->size += cursor->size+1;
            cursor->size = 0;
            //delete cursor
            removeInternal(parent->key[leftSibling], parent, cursor);
            //cout<<"Merged with left sibling\n";

        }
        else if(rightSibling <= parent->size)
        {
            //cursor + parent key + rightnode
            Node *rightNode = parent->ptr[rightSibling];
            cursor->key[cursor->size] = parent->key[rightSibling-1];
            for(int i = cursor->size+1, j = 0; j < rightNode->size; j++)
            {
                cursor->key[i] = rightNode->key[j];
            }
            for(int i = cursor->size+1, j = 0; j < rightNode->size+1; j++)
            {
                cursor->ptr[i] = rightNode->ptr[j];
                rightNode->ptr[j] = NULL;
            }
            cursor->size += rightNode->size+1;
            rightNode->size = 0;
            //delete cursor
            removeInternal(parent->key[rightSibling-1], parent, rightNode);
            //cout<<"Merged with right sibling\n";
        }
    }
public:
    BPlusTree()
    {
        // Initialize the B+ tree
        root = NULL;
    }
    Node *search(float x)
    {
        // search logic
        if (root == NULL) // don't search is its empty tree
        {
        }
        else
        {
            Node *cursor = root;
            int count = 1;
            // in the following while loop, cursor will travel to the leaf node possibly consisting the key
            while (cursor->isLeaf == false)
            {
                for (int i = 0; i < cursor->size; i++)
                {
                    if (x < cursor->key[i].key_value) // less than the key, that means we've got it, its within this node
                    {
                        cursor = cursor->ptr[i]; // next node is followed by pointer of the key
                        break;                   // break from for loop
                    }

                    if (i == cursor->size - 1) // the last key already and x is still more than key, must go to next internal node
                    {
                        cursor = cursor->ptr[i + 1]; // go to the next internal node
                        break;                       // break from for loop
                    }
                } // end for, after this, final cursor should be on the leaf node
                count++;
            }
            cout << "Number of index nodes accessed: " << count << "\n";
            // We search for the key if it exists

            for (int i = 0; i < cursor->size; i++) // this is the final cursor (node)
            {
                if (cursor->key[i].key_value == x)
                {
                    cout << "Number of duplicates: "<< cursor->key[i].secondary_key->size(); // print number of data blocks
                    cout << "\n";
                    /*for (int j = 0; j < cursor->key[i].secondary_key.size(); ++j) {
                        printf("Data Block: ");
                        printf("%p", (uchar *) cursor->key[i].secondary_key[j]); //print whatever key stored in duplicate_keys (unsigned character)
                        printf("\n");
                        cout << (*(Record *) cursor->key[i].add_vect[j]).tconst << "\n"; //print out the character string that follows add_vect[j] is block address!! WHAT ABOUT FIRST BLOCK?
                    }*/
                    return cursor; // this is the leaf node it is in :D , done
                }
            }
            // << "Not found\n"; // if this happens, means at x is not in the leaf node, not in b+ tree
        }

        return nullptr;
    }

    // vector<keys_struct>searchRange(float start,float end){
    //     return ;
    // }

    void insert(Record* record)
    {
        keys_struct entry;
        entry.key_value = record->fg_pct_home;
        entry.secondary_key->push_back(record);
        // Tree is empty
        if (root == NULL)
        {
            // cout << "root is null" << endl;
            root = new Node;
            root->key[0] = entry;
            root->isLeaf = true;
            root->size = 1;
            return;
        }
        else
        // Tree is not empty
        {
            Node *cursor = root;
            Node *parent;

            // Check if key in tree
            Node *searcher;
            searcher = search(entry.key_value);
            if (searcher != nullptr)
            {
                for (int i = 0; i < searcher->size; i++)
                {
                    if (searcher->key[i].key_value == entry.key_value)
                    {
                        // push entire secondary key without checking like FAQ 9
                        if (entry.secondary_key->size() > 0)
                            searcher->key[i].secondary_key->push_back(record);
                        break;
                    }
                }
                // cout << "how many searches" << endl;
                return;
            }
            // Search for node to insert new key
            while (cursor->isLeaf != true)
            {
                // cout << "root is not leaf" << endl;
                parent = cursor;
                for (int i = 0; i < cursor->size; i++)
                {
                    // Search for key > entry key, go left ptr of key
                    if (entry.key_value < cursor->key[i].key_value)
                    {
                        cursor = cursor->ptr[i];
                        break;
                    }
                    // If entry key is greater than all keys in node
                    if (i == cursor->size - 1)
                    {
                        cursor = cursor->ptr[i + 1];
                        break;
                    }
                }
            }

            // Attempt to insert the new key
            // If node is not full
            if (cursor->size < MAX_KEYS_NODE)
            {
                // cout << "cursor is not full" << endl;
                int i = 0;
                while (cursor->key[i].key_value < entry.key_value && i < cursor->size)
                    i++;
                for (int j = cursor->size; j > i; j--)
                {
                    cursor->key[j] = cursor->key[j - 1];
                    // If node is not full, right ptr of leaf node will be null, except ptr to next node
                    cursor->ptr[j] = cursor->ptr[j - 1];
                }
                cursor->key[i] = entry;
                cursor->size++;
                // cursor->ptr[i] = entryptr;
                // need a ptr to record
            }
            else
            {
                // Node is full
                // Create a new leaf node
                // cout << "node is full" << endl;
                Node *newLeaf = new Node;
                // Create a virtual node size MAX_KEYS_NODE + 1
                keys_struct virtualNode[MAX_KEYS_NODE + 1];
                for (int i = 0; i < MAX_KEYS_NODE; i++)
                {
                    virtualNode[i] = cursor->key[i];
                }
                int i = 0, j;
                // Search for position to insert entry, maximum is at MAX+1
                while (virtualNode[i].key_value < entry.key_value && i < MAX_KEYS_NODE)
                    i++;
                for (int j = MAX_KEYS_NODE; j > i; j--)
                {
                    virtualNode[j] = virtualNode[j - 1];
                }
                virtualNode[i] = entry;
                // default isLeaf = true but just in case
                newLeaf->isLeaf = true;
                // Split the cursor node
                cursor->size = (MAX_KEYS_NODE + 1) / 2;
                newLeaf->size = (MAX_KEYS_NODE + 1) - cursor->size;
                // temp node for later
                Node *temp = cursor->ptr[MAX_KEYS_NODE];
                // place keys into leafnodes, until new key
                // Scenario 1: new key is in the left node
                if (i < cursor->size)
                {
                    // cout << "leftnode key" << endl;
                    //  move all keys and ptr into larger node first
                    for (int j = 0, k = cursor->size; j < newLeaf->size; j++, k++)
                    {
                        newLeaf->key[j] = virtualNode[k];
                        newLeaf->ptr[j] = cursor->ptr[k - 1];
                        // All keys to the right of i are shifted right by 1
                        // k has no termination condition, it should never = MAX+1
                    }
                    // move all keys and ptr to the right of new key
                    for (int j = cursor->size - 1; j > i; j--)
                    {
                        cursor->key[j] = cursor->key[j - 1];
                        cursor->ptr[j] = cursor->ptr[j - 1];
                    }
                    // insert new key and ptr at i
                    cursor->key[i] = entry;
                    // cursor->ptr[i] = entryptr
                }
                else
                {
                    // cout << "rightnode key" << endl;
                    //  Scenario 2: new key is in the right node
                    //  move all keys and ptr to the left of i into the right node
                    int j = cursor->size, k = 0;
                    while (j < i && k < newLeaf->size)
                    {
                        newLeaf->key[k] = cursor->key[j];
                        newLeaf->ptr[k] = cursor->ptr[j];
                        k++;
                        j++;
                    }
                    // insert newkey and newptr
                    newLeaf->key[k] = entry;
                    // newLeaf->ptr[k] = entryptr;
                    k++;
                    // cout << "entryptr added" << endl;
                    //  insert the rest of the keys and ptrs
                    while (j < MAX_KEYS_NODE && k < newLeaf->size)
                    {
                        newLeaf->key[k] = cursor->key[j];
                        newLeaf->ptr[k] = cursor->ptr[j];
                        k++;
                        j++;
                    }
                }
                // cout << "nodecleaner reached" << endl;
                //  clean left node after cursorsize
                nodeCleaner(cursor);
                // New leafnode points to next node
                // Cursor points to new leafnode
                newLeaf->ptr[MAX_KEYS_NODE] = temp;
                cursor->ptr[MAX_KEYS_NODE] = newLeaf;
                // modify the parent
                // Scenario 1: cursor is root node
                if (cursor == root)
                {
                    // Create a new root
                    // rootSplitter(cursor, newLeaf, newLeaf->key[0]);
                    // cout << "call root splitter" << endl;
                    rootSplitter(cursor, newLeaf, newLeaf->key[0]);
                    // cout << "created new root" << endl;
                }
                else
                {
                    // Scenario 2: Cursor is not a root node
                    // Insert new key in parent node
                    insertInternal(newLeaf->key[0], parent, newLeaf);
                }
            }
        }
    }

    void remove(keys_struct x)
    {
        //delete logic
        if(root==NULL)
        {
            //cout<<"Tree empty\n";
        }
        else
        {
            Node* cursor = root;
            Node* parent;
            int leftSibling, rightSibling;
            //in the following while loop, cursor will will travel to the leaf node possibly consisting the key
            while(cursor->isLeaf == false)
            {
                for(int i = 0; i < cursor->size; i++)
                {
                    parent = cursor;
                    leftSibling = i-1; //leftSibling is the index of left sibling in the parent node
                    rightSibling =  i+1; //rightSibling is the index of right sibling in the parent node
                    if(x.key_value < cursor->key[i].key_value)
                    {
                        cursor = cursor->ptr[i];
                        break;
                    }
                    if(i == cursor->size - 1)
                    {
                        leftSibling = i;
                        rightSibling = i+2;
                        cursor = cursor->ptr[i+1];
                        break;
                    }
                }
            }
            //in the following for loop, we search for the key if it exists
            bool found = false;
            int pos;
            for(pos = 0; pos < cursor->size; pos++)
            {
                if(cursor->key[pos].key_value == x.key_value)
                {
                    found = true;
                    break;
                }
            }
            if(!found)//if key does not exist in that leaf node
            {
                //cout<<"Not found\n";
                return;
            }
            //deleting the key
            for(int i = pos; i < cursor->size; i++)
            {
                cursor->key[i] = cursor->key[i+1];
            }
            cursor->size--;
            if(cursor == root)//if it is root node, then make all pointers NULL
            {
                cout<<"Deleted "<<x.key_value << " " <<  "from leaf node successfully\n";
                for(int i = 0; i < MAX_KEYS_NODE+1; i++)
                {
                    cursor->ptr[i] = NULL;
                }
                if(cursor->size == 0)//if all keys are deleted
                {
                    //cout<<"Tree died\n";
                    delete[] cursor->key;
                    delete[] cursor->ptr;
                    delete cursor;
                    cout << "Deleted 1" <<"\n";
                    root = NULL;
                }
                return;
            }
            cursor->ptr[cursor->size] = cursor->ptr[cursor->size+1];
            cursor->ptr[cursor->size+1] = NULL;
            cout<<"Deleted "<<x.key_value << " " <<" from leaf node successfully\n";
            if(cursor->size >= (MAX_KEYS_NODE+1)/2)//no underflow
            {
                return;
            }
            //cout<<"Underflow in leaf node!\n";
            //underflow condition
            //first we try to transfer a key from sibling node
            //check if left sibling exists
            if(leftSibling >= 0)
            {
                Node *leftNode = parent->ptr[leftSibling];
                //check if it is possible to transfer
                if(leftNode->size >= (MAX_KEYS_NODE+1)/2+1)
                {
                    //make space for transfer
                    for(int i = cursor->size; i > 0; i--)
                    {
                        cursor->key[i] = cursor->key[i-1];
                    }
                    //shift pointer to next leaf
                    cursor->size++;
                    cursor->ptr[cursor->size] = cursor->ptr[cursor->size-1];
                    cursor->ptr[cursor->size-1] = NULL;
                    //transfer
                    cursor->key[0] = leftNode->key[leftNode->size-1];
                    //shift pointer of leftsibling
                    leftNode->size--;
                    leftNode->ptr[leftNode->size] = cursor;
                    leftNode->ptr[leftNode->size+1] = NULL;
                    //update parent
                    parent->key[leftSibling] = cursor->key[0];
                    //cout<<"Transferred "<<cursor->key[0].key_value << " " << cursor->key[0].add_vect[0] <<" from left sibling of leaf node\n";
                    return;
                }
            }
            if(rightSibling <= parent->size)//check if right sibling exist
            {
                Node *rightNode = parent->ptr[rightSibling];
                //check if it is possible to transfer
                if(rightNode->size >= (MAX_KEYS_NODE+1)/2+1)
                {
                    //shift pointer to next leaf
                    cursor->size++;
                    cursor->ptr[cursor->size] = cursor->ptr[cursor->size-1];
                    cursor->ptr[cursor->size-1] = NULL;
                    //transfer
                    cursor->key[cursor->size-1] = rightNode->key[0];
                    //shift pointer of rightsibling
                    rightNode->size--;
                    rightNode->ptr[rightNode->size] = rightNode->ptr[rightNode->size+1];
                    rightNode->ptr[rightNode->size+1] = NULL;
                    //shift conent of right sibling
                    for(int i = 0; i < rightNode->size; i++)
                    {
                        rightNode->key[i] = rightNode->key[i+1];
                    }
                    //update parent
                    parent->key[rightSibling-1] = rightNode->key[0];
                    //cout<<"Transferred "<<cursor->key[cursor->size-1].key_value <<" "<< cursor->key[cursor->size-1].add_vect[0] <<" from right sibling of leaf node\n";
                    return;
                }
            }
            //must merge and delete a node
            if(leftSibling >= 0)//if left sibling exist
            {
                Node* leftNode = parent->ptr[leftSibling];
                // transfer all keys to leftsibling and then transfer pointer to next leaf node
                for(int i = leftNode->size, j = 0; j < cursor->size; i++, j++)
                {
                    leftNode->key[i] = cursor->key[j];
                }
                leftNode->ptr[leftNode->size] = NULL;
                leftNode->size += cursor->size;
                leftNode->ptr[leftNode->size] = cursor->ptr[cursor->size];

                //cout<<"Merging two leaf nodes\n";
                removeInternal(parent->key[leftSibling],parent,cursor);// delete parent node key
                delete[] cursor->key;
                delete[] cursor->ptr;
                delete cursor;
                cout << "Deleted 1" <<"\n";
            }
            else if(rightSibling <= parent->size)//if right sibling exist
            {
                Node* rightNode = parent->ptr[rightSibling];
                // transfer all keys to cursor and then transfer pointer to next leaf node
                for(int i = cursor->size, j = 0; j < rightNode->size; i++, j++)
                {
                    cursor->key[i] = rightNode->key[j];
                }
                cursor->ptr[cursor->size] = NULL;
                cursor->size += rightNode->size;
                cursor->ptr[cursor->size] = rightNode->ptr[rightNode->size];
                //cout<<"Merging two leaf nodes\n";
                removeInternal(parent->key[rightSibling-1],parent,rightNode);// delete parent node key
                delete[] rightNode->key;
                delete[] rightNode->ptr;
                delete rightNode;
                cout << "Deleted 1 \n";
            }
        }
    }

    vector<keys_struct> searchRange(float keymin, float keymax){
        Node *leftBound = root, *rightBound = root;
        vector<keys_struct> results;
        vector<Node*> visited;
        if(root==NULL) return results;

        int count = 1;
        while(rightBound->isLeaf == false){
            for (int i = 0; i < rightBound->size; i++)
            {
                if (keymax < rightBound->key[i].key_value)
                {
                    rightBound = rightBound->ptr[i];
                    break;
                }
                if (i == rightBound->size - 1)
                {
                    rightBound = rightBound->ptr[i + 1];
                    break;
                }
            }
            visited.push_back(rightBound);
            count++;
        }
        while(leftBound->isLeaf == false){
            //set parent of leftbound before going into next node
            //reversed search, start from right
            for(int i = leftBound->size-1; i>=0 ;i--){
                if(keymin > leftBound->key[i].key_value){
                    leftBound = leftBound->ptr[i+1];
                    break;
                }
                if(i == 0){
                    leftBound = leftBound->ptr[i];
                    break;
                }
                for(int i =0; i<visited.size();i++){
                    if(leftBound == visited[i]){
                        continue;
                    }
                }
                count++;
            }
        }
        Node* cursor = leftBound;
        int i = 0;
        while(i<cursor->size && cursor->key[i].key_value < keymin) i++;
        while(cursor != rightBound){
            //theoretically keymax should never trigger
            while(i<cursor->size && keymin<=cursor->key[i].key_value){
                results.push_back(cursor->key[i]);
                i++;
            }
            cursor = cursor->ptr[MAX_KEYS_NODE];
            if(cursor!=rightBound){
                count++;
            }
            i = 0;
        }
        while(i<cursor->size && cursor->key[i].key_value <= keymax){
            results.push_back(cursor->key[i]);
            i++;
        }
        cout << "Number of index nodes accessed: " << count << endl;
        return results;
    }

    void removeRange(float keymin, float keymax)
    {
        if(root == NULL) return;
        vector<keys_struct> hitList;
        hitList = searchRange(keymin, keymax);
        while (!hitList.empty())
        {
            remove(hitList.front());
            hitList.erase(hitList.begin());
        }
    }

    void displayTree(Node *cursor, bool isRoot)
    {
        if (cursor != NULL)
        {
            queue<Node *> currentLevel;
            queue<Node *> nextLevel;

            currentLevel.push(cursor);
            int level = 0;

            while (!currentLevel.empty())
            {
                cursor = currentLevel.front();
                currentLevel.pop();

                if(cursor == nullptr || cursor==NULL){
                    cout << cursor->getKey() << endl;
                }

                for (int i = 0; i < cursor->size; i++)
                    cout << cursor->key[i].key_value << " ";

                if (level == 0)
                    cout << "(Root) ";
                else if (cursor->isLeaf)
                    cout << "(Leaf) ";
                else
                    cout << "(Internal) ";

                if (!cursor->isLeaf)
                {
                    for (int i = 0; i < cursor->size + 1; i++)
                    {
                        nextLevel.push(cursor->ptr[i]);
                    }
                }

                if (currentLevel.empty())
                {
                    swap(currentLevel, nextLevel);
                    cout << "\n";
                    level++;
                }
            }
        }
    }

    void getKey(Node *cursor)
    {
        for (int i = 0; i < MAX_KEYS_NODE; i++)
        {
            cout << cursor->key[i].key_value << endl;
        }
    }

    Node *getRoot()
    {
        return root;
    }
};

// int main()
// {
//     BPlusTree bptree;

//     keys_struct *dummyArray = new keys_struct[300];

//     // Create dummy data
//     for (int i = 0; i < 17; i++)
//     {
//         dummyArray[i].key_value = i;
//         bptree.insert(dummyArray[i]);
//     }

//     //bptree.getKey(bptree.getRoot());

//     bptree.displayTree(bptree.getRoot());

//     // bptree.print();

//     return 0;
// }
#endif