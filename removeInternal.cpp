void removeInternal (keys_struct x, Node* cursor, Node* child){
    if (cursor==root){
        if (cursor->size==1){ //if only 1 key is left
            for (i=0;i<2;i++){
                if (cursor->ptr[i]==child){//the first/ second pointer points to the child
                    delete[] child->key;
                    delete[] child->ptr;
                    delete child;
                    root=cursor->ptr[i] //become the root
                    delete[] cursor->key;
                    delete[] cursor-> ptr;
                    delete cursor;
                    return;
                }
            }
        }
    }
    //delete the KEY
    int j;
    for (int j=0; j<cursor->size;j++){
        if (cursor->key[j].key_value==x.key_value){
            break;
        }
    }
    for (int i=j; i<cursor->size; i++){ // continue from wherever j stopped; move keys forward
        cursor->key[i]=cursor->key[i+1]
    }
    //delete the empty NODE
    for (j=0; j<cursor->size+1; j++){
        if (cursor->ptr[j]==child){
            break;
        }
    }
    for (int i=j; i<cursor->size+1;i++){
        cursor->ptr[i]=cursor->ptr[i+1] //pointer moved forward
    }
    cursor->size--;
    if (cursor->size>=(MAX+1)/2-1){ //check for too little keys
        return;
    }
    if (cursor==root){
        return;
    }
    
    Node* parent= findParent(root,cursor);//redefine parent
    int leftsib, rightsib;
    for (i=0; i<parent->size+1; i++){
        if (parent->ptr[i]==cursor){
            leftsib=i-1;
            rightsib=i+1
            break;
        }
    }
    //Same logic as remove
    if (leftsib>=0){
        Node* leftNode=parent->ptr[leftsib];
        if (leftNode->size>=(MAX+1)/2){
            for (int i=cursor->size;i>0; i--){
                cursor->key[i]=cursor->key[i-1];
            }
            cursor->key[0]=parent->key[leftsib];
            parent->key[leftsib]=leftNode->key[leftNode->size-1]
            for (int i=cursor->size+1;i>0;i--){
                cursor->ptr[i]=cursor->ptr[i-1]
            }
            
            cursor->ptr[0]=leftNode->ptr[leftNode->size];
            cursor->size++;
            leftNode->size--;
            return;
        }
    }
    if (rightsib<=parent->size){
        Node* rightNode=parent->ptr[rightsib];
        if (rightNode->size>=(MAX+1)/2){
            cursor->key[cursor->size]=parent->key[i];
            parent->key[i]=rightNode->key[0]
            for (int i = 0; i < rightNode->size -1; i++)
            {
                rightNode->key[i] = rightNode->key[i+1];
            }
            for (int i = 0; i < rightNode->size; ++i)
            {
                rightNode->ptr[i] = rightNode->ptr[i+1];
            }
            
            cursor->size++;
            rightNode->size--;
            return;
        }
    }
    //need to do merging
    
    if (leftsib>=0){
        Node* leftNode=parent->ptr[leftsib];
        leftNode->key[leftNode->size]=parent->key[leftsib];
        
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
        removeInternal(parent->key[leftSibling], parent, cursor); //recursive
    }
    
    else if(rightsib<=parent->size){
        Node *rightNode=parent->ptr[rightsib];
        cursor->key[cursor->size]=parent->key[rightsib-1];
        
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
        
        removeInternal(parent->key[rightSibling-1], parent, rightNode);
    }
}
        


