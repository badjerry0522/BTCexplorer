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

void init_set_control(struct set_control *sc,int set_num,int lines,int linesize,uint8_t *d){
    struct set_control *sp;
    for(int i=0;i<set_num;i++){
        sp=&(sc[i]);
        for(int j=0;j<lines;j++){
            sp->l[j].valid=0;
            sp->l[j].p=&(d[i*SET_SIZE+j*LINE_SIZE]);
        }
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
    
    if(type==FULL){   //type=FULL
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
    else{ //type=NORMAL
        if(Csize<SET_SIZE){
            close(fhandle);
            return INVALID_ARG; //C is too small to hold one set
        } 
        d=(uint8_t*)malloc((1<<logC));
        if(d==NULL){
            close(fhandle);
            return CACHE_OUT_MEMORY;
        }
        log_set_num=logC-LOG_SET_SIZE;
        set_num=1<<log_set_num;
        sc=(struct set_control *)malloc(sizeof(struct set_control)*set_num);
        if(sc==NULL){
            close(fhandle);
            free(d):
            return CACHE_OUT_MEMORY;
        }
        init_set_control(sc,set_num,LINES,LINE_SIZE,d);
    }

}

ERROR_CODE cache::load(uint64_t index,uint8_t *p){
    uint8_t *p0;
    uint64_t offset;
    if(index>max_index) return INVALID_ARG;
    offset=index*Esize;
    if(type==FULL){
        p0=&(d[offset]);
        memcpy(p,p0,Esize);
        return NO_ERROR;
    }
    else if(type==NONE){
        uint64_t seek_result=lseek(fhandle,offset,SEEK_SET);
        int read_result=read(fhandle,p,Esize);
        if((seek_result==offset)&&(read_result==Esize)) return NO_ERROR;
        return ERROR_FILE;
    }
    else{  //NORMAL

    }
}

cache::~cache(){
    if(fhandle!=1) close(fhandle);
    if(d!=NULL) free(d);
    if(sc!=NULL) free(sc);

}

