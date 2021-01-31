#include "../include/tran_vec.h"

tran_vec::tran_vec(){
    cur=0;
}

int tran_vec::size(){
    return tv.size();
}

ERROR_CODE tran_vec::push_back(TRAN_SEQ t){
    if(tv.size()!=0){
        TRAN_SEQ last_seq=tv[tv.size()-1];
        if(last_seq>=t) return INVALID_ARG;
    }
    tv.push_back(t);
    return NO_ERROR;
}
TRAN_SEQ tran_vec::begin(ORDER o){
    if(tv.size()==0) return NULL_SEQ;
    if(o==FIRST){
        cur=0; last_index=tv.size()-1;step=1;
        return tv[cur];
    }
    cur=tv.size()-1;last_index=0;step=-1;
    return tv[cur];
}
TRAN_SEQ tran_vec::begin(){
    return begin(FIRST);
}
int tran_vec::isIn(TRAN_SEQ seq){
    for(int i=0;i<tv.size();i++){
        if(tv[i]==seq) return i;
    }
    return -1;
}
TRAN_SEQ tran_vec::begin(TRAN_SEQ seq,DIRECTION d,ORDER o){
    if(tv.size()==0) return NULL_SEQ;
    int index=isIn(seq);
    if(index==-1) return NULL_SEQ;
    if(d==BEFORE){
        if(index==0) return NULL_SEQ;
        if(o==FIRST){
            cur=0;last_index=index-1;step=1;
        }else{
            cur=index-1;last_index=0;step=-1;
        }
    }
    else{ //AFTER
        if(index==tv.size()-1) return NULL_SEQ;
        if(o==FIRST){
            cur=index+1;last_index=tv.size()-1;step=1;
        }
        else{
            cur=tv.size()-1;last_index=index+1;step=-1;
        }
    }
    return tv[cur];
}
TRAN_SEQ tran_vec::next(){
    cur+=step;
    if(step==1){
        if(cur<=last_index) return tv[cur];
        step=0;
        return NULL_SEQ;
    }
    else{
        if(cur>=last_index) return tv[cur];
        step=0;
        return NULL_SEQ;
    }
    return NULL_SEQ;

}
tran_vec::~tran_vec(){
    
}