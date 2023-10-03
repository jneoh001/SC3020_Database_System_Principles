//
//  SC3020.cpp
//  
//
//  Created by Si En on 19/9/23.
//
#include "SC3020.hpp"
#include <iostream>
#include <time.h>

Node* search(float x)
   {
       //search logic
       if(root==NULL) // don't search is its empty tree
       { }
       else{
           Node* cursor = root;
           //in the following while loop, cursor will travel to the leaf node possibly consisting the key
           while(cursor->IS_LEAF == false)
           {
               for(int i = 0; i < cursor->size; i++)
               {
                   if(x < cursor->key[i].key_value) // less than the key, that means we've got it, its within this node
                   {
                       cursor = cursor->ptr[i]; //next node is followed by pointer of the key
                       break; //break from for loop
                   }
                   
                   if(i == cursor->size - 1) //the last key already and x is still more than key, must go to next internal node
                   {
                       cursor = cursor->ptr[i+1]; //go to the next internal node
                       break; //break from for loop
                   }
               } // end for, after this, final cursor should be on the leaf node
           }
           //We search for the key if it exists
           
           for(int i = 0; i < cursor->size; i++) // this is the final cursor (node)
                       {
                           if(cursor->key[i].key_value == x)
                           {
                                   printf("Number of Data Blocks: %d", cursor->key[i].add_vect.size()); //print number of data blocks
                                   printf("\n");
                                   for (int j = 0; j < cursor->key[i].add_vect.size(); ++j) {
                                       printf("Data Block: ");
                                       printf("%p", (uchar *) cursor->key[i].add_vect[j]); //print whatever key stored in duplicate_keys (unsigned character)
                                       printf("\n");
                                       cout << (*(Record *) cursor->key[i].add_vect[j]).tconst << "\n"; //print out the character string that follows add_vect[j] is block address!! WHAT ABOUT FIRST BLOCK?
                                   }
                               }
                               return cursor; //this is the leaf node it is in :D , done
                           }
                        cout<<"Not found\n"; // if this happens, means at x is not in the leaf node, not in b+ tree
                        return nullptr;
                       }

    
    
    // reference to add_vect/ duplicate_key idea

    /*Node* searchRes;
                searchRes = search(x.key_value); //will return a leaf node which contains; flag helps in terms of not progressing to the next part of printing out
                if ( searchRes != nullptr){ //not empty tree
                    for(int i = 0; i < searchRes->size; i++)
                    {
                        if(searchRes->key[i].key_value == x.key_value) // if the value in the btree == what we want to insert already, put it in add_vect
                        {
                            searchRes->key[i].add_vect.push_back(x.add_vect[0]); //add_vect to the key (address added)
                            break; //break for
                        }
                    }
                    return; // return (nth else to do to insert the desired value x)
                }
    
    //additional code that could help understand

    void *recordAddress = (uchar *)blocksInMemory.at(blockAddress) + offset;
    //         cout << (*(Record *) recordAddress).averageRating << (*(Record *) recordAddress).tconst << " at " << recordAddress << '\n';
             float num = (*(Record *) recordAddress).averageRating;
             keys_struct key;
             key.key_value = num;
             key.add_vect.push_back((uchar *) blocksInMemory.at(blockAddress) + offset); //addvect contains the block address
             node.insert(key);
