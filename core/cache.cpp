#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include "../include/core_type.h"
#include "../include/cache.h"
using namespace std;
int ispowerof2(uint64_t d){
    if(d==0) return 0;
    int bit1count=0;
    for(int i=0;i<64;i++){
        if(d==0) break;
        if(d&0x1==1) bit1count++;
        d=d>>1;
    }
    if(bit1count==1) return 1;
    else return 0;
}

int log2(uint64_t d, ERROR_CODE *err){
    if(d==0) return 0;
    int bit1count=0;
    int i;
    for(i=0;i<64;i++){
        if(d==0) break;
        if(d&0x1==1) bit1count++;
        d=d>>1;
    }
    if(bit1count==1){
        *err=NO_ERROR;
        return i;
    }
    else {
        *err=INVALID_ARG;
        return 0;
    }
}

cache::cache(){
    this->sc=NULL;
    this->d=NULL;
    this->fhandle=-1;
}
ERROR_CODE cache::init(char *filename,int logC,int logE,uint64_t _max_index){
    if(logC==0){
        type=NONE;
    }
    else if(logC==0xFFFFFFFF){
        type=FULL;
    }
    else{
        type=NORMAL;
    }
    this->Esize=1<<logE;
    this->max_index=_max_index;
    Csize=1<<logC;

    uint64_t filesize=Esize*max_index;
    //Check File Size=max_index*Esize
    struct stat statbuf;
    int ret;

    ret = stat(filename,&statbuf);
    if(ret != 0) return ERROR_FILE;
    if(statbuf.st_size!=filesize) return ERROR_FILE;
    this->fhandle=open(filename,O_RDONLY|O_BINARY);
    if(fhandle==-1){
        return ERROR_FILE;
    }
    if(type==NONE) return NO_ERROR;

    if(Csize>=filesize) type=FULL;
    if(type==NORMAL){    
        d=(uint8_t*)malloc((1<<logC));
        if(d==NULL){
            close(fhandle);
            return CACHE_OUT_MEMORY;
        }
    }
    else{   //type=FULL
        d=(uint8_t *)malloc(filesize);
        if(d==NULL){
            close(fhandle);
            return CACHE_OUT_MEMORY;
        }
        uint64_t readret=read(fhandle, d, filesize);
        if(readret!=filesize){
            free(d);
            close(fhandle);
            return ERROR_FILE;
        }
        close(fhandle);
        fhandle=-1;
        return NO_ERROR;
    }

}

ERROR_CODE cache::load(uint64_t,uint8_t *){

}

cache::~cache(){

}

