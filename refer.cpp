#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<climits>
#include "memory-pool.h"

using namespace std;


 void insert(keys_struct x)
    {
        //insert logic
        if(root==NULL)
        {
            root = new Node;
            root->key[0] = x;
            root->IS_LEAF = true;
            root->size = 1;
            //cout<<"Created root\nInserted "<<  x.key_value << " " << x.add_vect[0] <<" successfully\n";
        }
        else
        {
            Node* cursor = root;
            Node* parent;

            Node* searchRes;
            searchRes = search(x.key_value, false, false);
            if ( searchRes != nullptr){
                for(int i = 0; i < searchRes->size; i++)
                {
                    if(searchRes->key[i].key_value == x.key_value)
                    {
                        searchRes->key[i].add_vect.push_back(x.add_vect[0]);
                        break;
                    }
                }
                return;
            }
            //in the following while loop, cursor will travel to the leaf node possibly consisting the key
            while(cursor->IS_LEAF == false)
            {
                parent = cursor;
                for(int i = 0; i < cursor->size; i++)
                {
                    if(x.key_value < cursor->key[i].key_value)
                    {
                        cursor = cursor->ptr[i];
                        break;
                    }
                    if(i == cursor->size - 1)
                    {
                        cursor = cursor->ptr[i+1];
                        break;
                    }
                }
            }
            //now cursor is the leaf node in which we'll insert the new key
            if(cursor->size < MAX)
            {
                //if cursor is not full
                //find the correct position for new key
                int i = 0;
                while(x.key_value > cursor->key[i].key_value && i < cursor->size) i++;
                //make space for new key
                for(int j = cursor->size;j > i; j--)
                {
                    cursor->key[j] = cursor->key[j-1];
                }
                cursor->key[i] = x;
                cursor->size++;
                cursor->ptr[cursor->size] = cursor->ptr[cursor->size-1];
                cursor->ptr[cursor->size-1] = NULL;
                //cout<<"Inserted "<< x.key_value << " " << x.add_vect[0] <<" successfully\n";
            }
            else
            {
                //cout<<"Inserted "<< x.key_value << " " << x.add_vect[0] <<" successfully\n";
                //cout<<"Overflow in leaf node!\nSplitting leaf node\n";
                //overflow condition
                //create new leaf node
                Node* newLeaf = new Node;
                //create a virtual node and insert x into it
                keys_struct virtualNode[MAX+1];
                for(int i = 0; i < MAX; i++)
                {
                    virtualNode[i] = cursor->key[i];
                }
                int i = 0, j;
                while(x.key_value > virtualNode[i].key_value && i < MAX) i++;
                //make space for new key
                for(int j = MAX+1;j > i; j--)
                {
                    virtualNode[j] = virtualNode[j-1];
                }
                virtualNode[i] = x;
                newLeaf->IS_LEAF = true;
                //split the cursor into two leaf nodes
                cursor->size = (MAX+1)/2;
                newLeaf->size = MAX+1-(MAX+1)/2;
                //make cursor point to new leaf node
                cursor->ptr[cursor->size] = newLeaf;
                //make new leaf node point to the next leaf node
                newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];
                cursor->ptr[MAX] = NULL;
                //now give elements to new leaf nodes
                for(i = 0; i < cursor->size; i++)
                {
                    cursor->key[i] = virtualNode[i];
                }
                for(i = 0, j = cursor->size; i < newLeaf->size; i++, j++)
                {
                    newLeaf->key[i] = virtualNode[j];
                }
                //modify the parent
                if(cursor == root)
                {
                    //if cursor is a root node, we create a new root
                    Node* newRoot = new Node;
                    newRoot->key[0] = newLeaf->key[0];
                    newRoot->ptr[0] = cursor;
                    newRoot->ptr[1] = newLeaf;
                    newRoot->IS_LEAF = false;
                    newRoot->size = 1;
                    root = newRoot;
                    //cout<<"Created new root\n";
                }
                else
                {
                    //insert new key in parent node
                    insertInternal(newLeaf->key[0],parent,newLeaf);
                }
            }
        }
    }