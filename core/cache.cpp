#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include<cmath>
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
//wrong
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
void print_hit(unsigned char *p,struct set_control *sc,uint64_t index_set,uint64_t index_hit,uint64_t offset_line,int Esize){
    //cout<<"index_set="<<index_set<<" index_hit="<<index_hit<<" offset_line="<<offset_line<<endl;
    memcpy(p,sc[index_set].l[index_hit].p+offset_line*Esize,Esize);
    //cout<<"memcpy complete"<<endl;
}
int check_is_hit(struct set_control *sc,uint64_t index_set,uint64_t tag){// if hit, return the NO. of line in the set
                                                                         // else return -1
    for(int i=0;i<4;i++){
        if((tag==sc[index_set].l[i].tag)&&(sc[index_set].l[i].valid!=0)) return i;
    }
    return -1;
}
int getleast(struct set_control *sc,uint64_t index_set){//return the index of the line with the fewest access_times
    int least=0;
    for(int i=0;i<4;i++){
        if(sc[index_set].l[i].access_times<least) least=i;
        if(sc[index_set].l[i].valid==0) return i;
    }
    return least;
}
void update_line(struct set_control *sc,uint64_t index_set,uint64_t tag,int hit_line,int hit_type,int dirty){
    if(hit_type==0){//MISS
        sc[index_set].l[hit_line].tag=tag;
        sc[index_set].l[hit_line].access_times=0;
        sc[index_set].l[hit_line].dirty=dirty;
        sc[index_set].l[hit_line].valid=1;
    }
    else{//HIT
        sc[index_set].l[hit_line].access_times++;
        sc[index_set].l[hit_line].dirty=dirty;
    }
}
void write_to_disk(struct set_control *sc,uint64_t index,uint64_t index_set,int hit_line,int log_line_size,int Esize,FILE *fout){
    uint64_t offset_line=((index>>log_line_size)<<log_line_size)*Esize;//offset in line
    int seek_result=fseek(fout,offset_line,SEEK_SET);
    fwrite(sc[index_set].l[hit_line].p,1,LINE_SIZE,fout);
    /*  no update in write/read, update status in update_line
    sc[index_set].l[hit_line].dirty=0;
    sc[index_set].l[hit_line].access_times++;
    */
}
void write_to_cache(struct set_control *sc,uint64_t index,uint64_t index_set,int hit_line,int index_line,int Esize,unsigned char *p){
    memcpy(sc[index_set].l[hit_line].p+(index_line*Esize),p,Esize);
}
ERROR_CODE read_from_disk(struct set_control *sc,uint64_t index,uint64_t index_set,//read the element inf to cache
    int hit_line,int log_line_size,int Esize,FILE *fin){
    uint64_t offset_line=((index>>log_line_size)<<log_line_size)*Esize;//the position of the start of the line in disk
    int seek_result=fseek(fin,offset_line,SEEK_SET);
    int fread_result=fread(sc[index_set].l[hit_line].p,1,LINE_SIZE,fin);//read in to cache
    return NO_ERROR;
}
cache::cache(){
    this->sc=NULL;
    this->d=NULL;
    this->fhandle=NULL;
}
ERROR_CODE cache::init(char *filename,int logC,int logE,uint64_t _max_index){
    //cout<<endl<<"filename="<<filename<<endl;
    if(logC==0){
        type=NONE;
    }
    else if(logC==0xFFFFFFFF){
        type=FULL;
    }
    else{
        type=NORMAL;
    }
    this->Esize=(uint64_t)1<<logE;
    this->max_index=_max_index;
    //cout<<"logc="<<logC<<endl;
    Csize=(uint64_t)1<<logC;
    //cout<<"Csize="<<Csize<<endl;

    uint64_t filesize=Esize*max_index;
    //cout<<"filesize="<<filesize<<endl;
    //Check File Size=max_index*Esize
    struct stat statbuf;
    int ret;

    ret = stat(filename,&statbuf);
    //cout<<"statbuf.st_size="<<statbuf.st_size<<endl;
    if(ret != 0) return ERROR_FILE;
    if(statbuf.st_size!=filesize) return ERROR_FILE;
    //cout<<"filename="<<filename<<endl;
    this->fhandle=fopen(filename,"rb");
    if(fhandle==NULL){
        //cout<<"fhandle=NULL"<<endl;
        return ERROR_FILE;
    }
    if(type==NONE) return NO_ERROR;
    if(Csize>=filesize) type=FULL;
    if(type==FULL){   //type=FULL
        d=(uint8_t *)malloc(filesize);
        if(d==NULL){
            fclose(fhandle);
            return CACHE_OUT_MEMORY;
        }
        uint64_t freadret=fread(d, Esize,max_index,fhandle);
        if(freadret!=filesize){
            free(d);
            fclose(fhandle);
            return ERROR_FILE;
        }
        fclose(fhandle);
        fhandle=NULL;
        return NO_ERROR;
    }
    //type=NORMAL
    cout<<"type=NORMAL"<<endl;
    if(Csize<SET_SIZE){
        fclose(fhandle);
        return INVALID_ARG; //C is too small to hold one set
    } 
    d=(uint8_t*)malloc(((uint64_t)1<<logC));
    if(d==NULL){
        fclose(fhandle);
        return CACHE_OUT_MEMORY;
    }
    log_set_num=logC-LOG_SET_SIZE;
    set_num=(uint64_t)1<<log_set_num;
    cout<<"set_num="<<set_num<<endl;
    sc=(struct set_control *)malloc(sizeof(struct set_control)*set_num);
    if(sc==NULL){
        fclose(fhandle);
        free(d);
        return CACHE_OUT_MEMORY;
    }
    init_set_control(sc,set_num,LINES,LINE_SIZE,d);
    return NO_ERROR;
}

ERROR_CODE cache::load(uint64_t index,unsigned char *p){
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
        uint64_t seek_result=fseek(fhandle,offset,SEEK_SET);
        int fread_result=fread(p,Esize,1,fhandle);
        if((seek_result==offset)&&(fread_result==Esize)) return NO_ERROR;
        return ERROR_FILE;
    }
    else if(type==NORMAL){
        //cout<<"index="<<index<<endl;
        ERROR_CODE err;
        int logE=log2(Esize);
        uint64_t log_line_size=22-logE;//log of number of elements each line contains
        //cout<<"log_line_size="<<log_line_size<<endl;
        set_mask=(((uint64_t)1<<(log_set_num))-(uint64_t)1);
        tag_mask=(((uint64_t)1<<(uint64_t)(64-log_set_num-log_line_size))-(uint64_t)1)<<(uint64_t)(log_set_num+log_line_size);
        uint64_t index_set=(index>>log_line_size)&set_mask;//store in which set 
        uint64_t tag=index&tag_mask;
        //cout<<"tag="<<tag<<endl;
        uint64_t index_line=index&((1<<(log_line_size))-1);//the element's position in the line
         

        int hit=0;
        int index_hit=check_is_hit(sc,index_set,tag);//check hit or not
        if(index_hit==-1) hit=0;
        else hit=1;
        int least=getleast(sc,index_set);//the line with the fewest access_times

        if(hit==0){
            index_hit=least;//replace the line fewest access_times
            read_from_disk(sc,index,index_set,index_hit,log_line_size,Esize,fhandle);
        }
        //cout<<"read from disk complete in load"<<endl;
        update_line(sc,index_set,tag,index_hit,hit,0);//update the line_control information
        //cout<<"update_lien complete in load"<<endl;
        print_hit(p,sc,index_set,index_hit,index_line,Esize);//copy the element to p
        //cout<<"print_hit in load complete"<<endl;
    }
    return NO_ERROR;
}

ERROR_CODE cache::store(uint64_t index,unsigned char *p){
    uint64_t offset;
    if(index>max_index) return INVALID_ARG;
    if(type==FULL){
        offset=index*Esize;
        memcpy(d+offset,p,Esize);
        fwrite(p,Esize,1,fhandle);
        return NO_ERROR;
    }
    if(type==NORMAL){
        int logE=(int)log2(Esize);
        uint64_t log_line_size=22-logE;//log of number of elements each line contains
        set_mask=((1<<(log_set_num))-1);
        //cout<<"set_num="<<set_num<<endl;
        //cout<<"set_mask="<<set_mask<<endl;
        tag_mask=((1<<(64-log_set_num-log_line_size))-1)<<(log_set_num+log_line_size);
        uint64_t index_set=(index>>log_line_size)&set_mask;//store in which set 
       // cout<<"index_set="<<index_set<<endl;
        uint64_t tag=index&tag_mask;
        //cout<<"tag="<<tag<<endl;
        uint64_t index_line=index&((1<<(log_line_size))-1);//the element's position in the line
        //cout<<"index_line="<<index_line<<endl;


        int index_hit=check_is_hit(sc,index_set,tag);
       // cout<<"index_hit="<<index_hit<<endl;
        int least=getleast(sc,index_set);
       // cout<<"least="<<least<<endl;


        if(index_hit==-1){//MISS
            index_hit=least;//the line with fewest access_times
            if(sc[index_set].l[index_hit].dirty==1){
                write_to_disk(sc,index,index_set,index_hit,log_line_size,Esize,fhandle);//write to disk
            }
            read_from_disk(sc,index,index_set,index_hit,log_line_size,Esize,fhandle);//read the line from disk
            write_to_cache(sc,index,index_set,index_hit,index_line,Esize,p);//change the specific element in the line
            update_line(sc,index_set,tag,index_line,0,1);//update line status
        }
        else{
            write_to_cache(sc,index,index_set,index_hit,index_line,Esize,p);//change the specific element in the line
            update_line(sc,index_set,tag,index_hit,1,1);//update line status
            //cout<<"update_line complete"<<endl;
        }
        return NO_ERROR;
    }
    if(type==NONE){
        uint64_t offset_line=index*Esize;
        int seek_result=fseek(fhandle,offset_line,SEEK_SET);
        fwrite(p,Esize,1,fhandle);
        return NO_ERROR;
    }
    return NO_ERROR;
}

cache::~cache(){
    if(type==NORMAL){//write all unsaved line in cache to disk
        for(uint64_t i=0;i<set_num;i++){
            for(uint64_t j=0;j<4;j++){
                if(sc[i].l[j].dirty==1){
                    uint64_t offset_line=sc[i].l[j].tag+(uint64_t)(i<<(22-(int)log2(Esize)));//begin pos of the line
                    fwrite(sc[i].l[j].p+offset_line,LINE_SIZE,1,fhandle);//write to disk
                }
            }
        }
    }
    if(fhandle!=NULL) fclose(fhandle);
    if(d!=NULL) free(d);
    if(sc!=NULL) free(sc);
    
}

