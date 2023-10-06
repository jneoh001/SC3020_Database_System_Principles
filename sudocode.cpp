sudocode
void removerangeinternal(max, min, cursor){
    curkey = findkey between max and min
        if not found then return
    removerangeinternal on leftptr of curkey
    if nextkey exists  
        if nextkey <= max
            nuke rightptr of curkey
            size--
        else
            removerangeinternal on rightptr of curkey
    if curkey index = 0 
        if ptr[0] notleaf
            removerangeinternal on ptr[0]
        else
            removerangeleafinternal on ptr[0]
    if curkey index = curkeysize-1  
        if ptr[size] notleaf
            removerangeinternal on ptr[size]
        else
            removerangeleafinternal on ptr[size]
    
}
for start to end{
    
}
for 0 to end{
    index all elements on array
        remove(key)
}

//update rightbound size
            rightBound->size = rightBound->size + start - end;
            //leftshift everything from end onwards to start
            for(int i=start;i<rightBound->size;i++){
                rightBound->key[i] = rightBound->key[i+start-end];
                rightBound->ptr[i] = rightBound->ptr[i+start-end];
            }
            //clean rightbound
            nodeCleaner(rightBound);

    void removeRange(float keymin, float keymax){
        //base case
        if(root == NULL) return;
        if(keymin>keymax){
            return;
        }
        Node* rightParent;
        Node* rightBound;
        rightBound = root;
        int rbLeftSib, rbRightSib;

        //search for upper/rightbound
        while(rightBound->isLeaf == false){
            //set parent of rightbound before going into next node
            rightParent = rightBound;
            for (int i = 0; i < rightBound->size; i++)
            {
                if (keymax < rightBound->key[i].key_value)
                {
                    rbLeftSib = i-1; rbRightSib = i+1;
                    rightBound = rightBound->ptr[i];
                    break;
                }
                if (i == rightBound->size - 1)
                {
                    rbLeftSib = i; rbRightSib = i+2;
                    rightBound = rightBound->ptr[i + 1];
                    break;
                }
            }
        }

        //search for lower/leftbound
        Node* leftParent;
        Node* leftBound;
        leftBound = root;
        int lbLeftSib, lbRightSib;
        while(leftBound->isLeaf == false){
            //set parent of leftbound before going into next node
            leftParent = leftBound;
            //reversed search, start from right
            for(int i = leftBound->size-1; i>=0 ;i--){
                if(keymin > leftBound->key[i].key_value){
                    lbLeftSib = i; lbRightSib = i+2;
                    leftBound = leftBound->ptr[i+1];
                    break;
                }
                if(i == 0){
                    lbLeftSib = i-1; lbRightSib = i+1;
                    leftBound = leftBound->ptr[i];
                    break;
                }
            }
        }

        vector <Node*> hitList;
        vector <keys_struct> lkeyHitList;
        vector <keys_struct> rkeyHitList;

        //Search for Scenario 3: leftNode and rightNode have 1 or more nodes between them
        if(rightBound != leftBound && leftBound->ptr[MAX_KEYS_NODE] != rightBound){
            //if Scenario 3: make a vector of all the nodes between left and rightbound
            Node* cursor = leftBound->ptr[MAX_KEYS_NODE];
            while(cursor != rightBound){
                hitList.push_back(cursor);
                cursor = cursor->ptr[MAX_KEYS_NODE];
            }
        }

        int end = 0;
        //search for end key = first bigger than max
        while(end<rightBound->size && rightBound->key[end].key_value<=keymax) end++;
        // search for start key = first bigger than or equal to min
        int start = 0;
        while (start < rightBound->size && rightBound->key[start].key_value < keymin)
            start++;

        //Not Scenario 1
        if(rightBound!=leftBound){
            //index all elements from start to size
            for(int i = start; i<leftBound->size;i++){
                lkeyHitList.push_back(leftBound->key[i]);
            }
        }

        //Scenario 1: rightbound == leftbound, bounds in same node
        if(rightBound == leftBound){
            //index all elements between start and end
            for(int i = start;i<end;i++){
                rkeyHitList.push_back(rightBound->key[i]);
            }
        }else{
            //Scenario 2 and 3: rightbound != leftbound
            // index everything before end
            for (int i = 0; i < end; i++)
            {
                rkeyHitList.push_back(rightBound->key[i]);
            }
        }

        while(!rkeyHitList.empty()){
            remove(rkeyHitList.back());
            rkeyHitList.pop_back();
        }

        while(!hitList.empty()){
            Node* rightSibling = hitList.back();
            hitList.pop_back();
            removeInternal(rightSibling->key[0], findParent(root, hitList.back()), rightSibling);
        }
    }