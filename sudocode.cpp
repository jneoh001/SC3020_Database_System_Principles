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