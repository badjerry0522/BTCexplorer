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
    //cout<<"init set control begin"<<endl;
    for(int i=0;i<set_num;i++){
      //  cout<<i<<" of "<<set_num<<endl;
        for(int j=0;j<lines;j++){
            sc[i].l[j].valid=0;
            sc[i].l[j].p=&(d[i*SET_SIZE+j*LINE_SIZE]);
        }
    }
}
void cache::print_hit(unsigned char *p){
    //cout<<"index_set="<<index_set<<" index_hit="<<index_hit<<" offset_line="<<offset_line<<endl;
    memcpy(p,sc[index_set].l[index_hit].p+index_line*Esize,Esize);
    //cout<<"memcpy complete"<<endl;
}
void cache::print_multi_hit(unsigned char *p,int len){
    memcpy(p,sc[index_set].l[index_hit].p+index_line*Esize,Esize*len);
}
int cache::check_is_hit(){// if hit, return the NO. of line in the set
                                                       // else return -1
    for(int i=0;i<4;i++){
        if(sc[index_set].l[i].valid==0) return -1;
        //cout<<sc[index_set].l[i].tag<<endl;
        if((tag==sc[index_set].l[i].tag)) return i;
    }
    return -1;
}
int cache::getleast(){//return the index of the line with the fewest access_times
    int least=0;
    for(int i=0;i<4;i++){
        if(sc[index_set].l[i].access_times<least) least=i;
        if(sc[index_set].l[i].valid==0) return i;
    }
    return least;
}
void cache::update_line(int hit_type,int dirty){
    if(hit_type==0){//MISS
        sc[index_set].l[index_hit].tag=tag;
        sc[index_set].l[index_hit].access_times=0;
        sc[index_set].l[index_hit].dirty=dirty;
        sc[index_set].l[index_hit].valid=1;
    }
    else{//HIT
        sc[index_set].l[index_hit].access_times++;
        sc[index_set].l[index_hit].dirty=dirty;
    }
}
void cache::write_to_disk(uint64_t index,FILE *fout){
    //cout<<"write to disk begin"<<endl;
    uint64_t offset_line=((index>>log_line_size)<<log_line_size)*Esize;//the first element in the line
    int seek_result=fseek(fout,offset_line,SEEK_SET);
    fwrite(sc[index_set].l[index_hit].p,1,LINE_SIZE,fout);
    /*  no update in write/read, update status in update_line
    sc[index_set].l[hit_line].dirty=0;
    sc[index_set].l[hit_line].access_times++;
    */
}
void cache::write_to_cache(unsigned char *p){
    memcpy(sc[index_set].l[index_hit].p+(index_line*Esize),p,Esize);
}
//read the element inf to cache
void cache::read_from_disk(uint64_t index,FILE *fin){
    uint64_t offset_line=((index>>log_line_size)<<log_line_size)*Esize;//the position of the start of the line in disk
    int seek_result=fseek(fin,offset_line,SEEK_SET);
    int fread_result=fread(sc[index_set].l[index_hit].p,1,LINE_SIZE,fin);//read in to cache
}
void cache::preparation(uint64_t index){
    index_set=(index>>log_line_size)&set_mask;//store in which set 
    tag=index&tag_mask;
    index_line=index&(((uint64_t)1<<(log_line_size))-1);//the element's position in the line
    //cout<<"log_line_size="<<log_line_size<<endl;
}

cache::cache(){
    this->sc=NULL;
    this->d=NULL;
    this->fhandle=NULL;
}
ERROR_CODE cache::init(char *filename,int logC,int _logE,uint64_t _max_index){
    strcpy(this->filename,filename);
    //cout<<"filename="<<filename<<endl;
    if(logC==0){
        type=NONE;
    }
    else if(logC==0xFFFFFFFF){
        type=FULL;
    }
    else{
        type=NORMAL;
    }
    logE=_logE;
    Esize=(uint64_t)1<<logE;

    Csize=(uint64_t)1<<logC;
    log_line_size=22-logE;//log of number of elements each line contains

    log_set_num=logC-(LOG_LINE_SIZE+LOG_LINES);
    set_num=(uint64_t)1<<log_set_num;

    //init mask
    set_mask=(((uint64_t)1<<(log_set_num))-(uint64_t)1);
    tag_mask=(((uint64_t)1<<(uint64_t)(64-log_set_num-log_line_size))-(uint64_t)1)<<(uint64_t)(log_set_num+log_line_size);
    this->max_index=_max_index;
    uint64_t filesize=Esize*max_index;
    //Check File Size=max_index*Esize
    struct stat statbuf;
    int ret;

    ret = stat(filename,&statbuf);
    if(ret != 0) return ERROR_FILE;
    //cout<<"stabuf.st_size="<<statbuf.st_size<<"  filesize="<<filesize<<endl;
    if(statbuf.st_size!=filesize) return ERROR_FILE;
    this->fhandle=fopen(filename,"rb+");
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
        //cout<<"fread begin"<<endl;
        uint64_t freadret=fread(d, Esize,max_index,fhandle);
        //cout<<"freadret="<<freadret<<"  filesize="<<filesize<<endl;
        //cout<<"fread end"<<endl;
//getchar();
        if(freadret*Esize!=filesize){
            free(d);
            fclose(fhandle);
            return ERROR_FILE;
        }
        fclose(fhandle);
        fhandle=NULL;
        return NO_ERROR;
    }
    //type=NORMAL
   // cout<<"type="<<type<<endl;
    if(Csize<SET_SIZE){
        fclose(fhandle);
        return INVALID_ARG; //C is too small to hold one set
    } 
    d=(uint8_t*)malloc(((uint64_t)1<<logC));
    if(d==NULL){
        fclose(fhandle);
        return CACHE_OUT_MEMORY;
    }
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
        ERROR_CODE err;
        /*
        index_set=(index>>log_line_size)&set_mask;
        tag=index&tag_mask;
        index_line=index&((1<<(log_line_size))-1);//the element's position in the line
        */
        preparation(index);
        //cout<<"index="<<index<<endl;
        //line_control *lp=&(sc[index_set]);

        hit=0;
        index_hit=check_is_hit();//check hit or not
        if(index_hit==-1) hit=0;
        else hit=1;

        if(hit==0){
            least=getleast();//the line with the fewest access_times
            index_hit=least;//replace the line fewest access_times
            if(sc[index_set].l[index_hit].dirty==1){

                write_to_disk(index,fhandle);//write to disk
            }
            read_from_disk(index,fhandle);
        }
        //else cout<<"HIT"<<endl;
        update_line(hit,0);//update the line_control information
        print_hit(p);//copy the element to p
    }
    return NO_ERROR;
}

ERROR_CODE cache::store(uint64_t index,unsigned char *p){
    //cout<<"store begin"<<endl;
    //fhandle=fopen(this->filename,"rb+");
    uint64_t offset;
    cout<<"index="<<index<<endl;
    if(index>max_index) return INVALID_ARG;
    if(type==FULL){
        //cout<<"FULL IN STORE"<<endl;
        offset=index*Esize;
        //cout<<"offset="<<offset<<endl;
        memcpy(d+offset,p,Esize);
        //fhandle=fseek(fhandle,0,SEEK_SET);
        //fwrite(p,Esize,1,fhandle);
        return NO_ERROR;
    }
    if(type==NORMAL){
        /*
        index_set=(index>>log_line_size)&set_mask;//store in which set 
        tag=index&tag_mask;
        index_line=index&((1<<(log_line_size))-1);//the element's position in the line
        */
        preparation(index);
        index_hit=check_is_hit();


        if(index_hit==-1){//MISS
           // cout<<"MISS"<<endl;
            int least=getleast();
            index_hit=least;//the line with fewest access_times
            if(sc[index_set].l[index_hit].dirty==1){
                write_to_disk(index,fhandle);//write to disk
            }
            read_from_disk(index,fhandle);//read the line from disk
            write_to_cache(p);//change the specific element in the line
            update_line(0,1);//update line status
        }
        else{
            //cout<<"HIT"<<endl;
            write_to_cache(p);//change the specific element in the line
            update_line(1,1);//update line status
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
ERROR_CODE cache::multiload(uint64_t index,int len,unsigned char *p){
    uint64_t index_start,index_end;
    index_start=index,index_end=index+len;
    if(len<LINE_SIZE/Esize){
        if((index_set)==((index_end>>log_line_size)&set_mask)){
            //start and end have same tag
            //means in the same line
            preparation(index);
            index_hit=check_is_hit();
            if(index_hit==-1){
                //MISS
                index_hit=getleast();
                if(sc[index_set].l[index_hit].dirty==1) write_to_disk(index,fhandle);
                read_from_disk(index,fhandle);
                update_line(0,0);
            }
            else{
                //HIT
                update_line(1,0);
            }
            print_multi_hit(p,len);
            return NO_ERROR;
        }
        else{
            uint64_t index_start_prev=index;//start index of first part
            uint64_t index_start_last=((index_start>>log_line_size)+1)<<log_line_size;//start index of last part
            int len_prev=index_start_last-index_start_prev;//length of first part
            int len_last=len-len_prev;//length of second part

            unsigned char *p1=(unsigned char *)malloc(Esize*len_prev);
            unsigned char *p2=(unsigned char *)malloc(Esize*len_last);
            //separately load multi elements from p1 and p2 from two different set 
            multiload(index_start_prev,len_prev,p1);
            multiload(index_start_last,len_last,p2);
            
            //copy them to p 
            memcpy(p,p1,Esize*len_prev);
            memcpy(p+len_prev,p2,Esize*len_last);

            free(p1),free(p2);
            return NO_ERROR;
        }
    }
}
cache::~cache(){
   // cout<<"~cache begin"<<endl;
    if(type==NORMAL){//write all unsaved line in cache to disk
    //cout<<"NORMAL in ~cache"<<endl;
//getchar();
        for(uint64_t i=0;i<set_num;i++){
            for(uint64_t j=0;j<4;j++){
                if(sc[i].l[j].dirty==1){
                    uint64_t offset_line=sc[i].l[j].tag+(uint64_t)(i<<log_line_size);//begin pos of the line
                    fseek(fhandle,offset_line,SEEK_SET);
                    fwrite(sc[i].l[j].p+offset_line,LINE_SIZE,1,fhandle);//write to disk
                }
            }
        }
    }
    else if(type==FULL){
      //  cout<<"FULL in ~cache"<<endl;
//getchar();
        fhandle=fopen(filename,"rb+");
        if(fhandle==NULL){
      //      cout<<"fhandle==NULL"<<endl;
        }
       // cout<<"fseek complete"<<endl;
//getchar();
        fwrite(d,Esize*max_index,1,fhandle);
    }
    if(fhandle!=NULL) fclose(fhandle);
    if(d!=NULL) free(d);
    if(sc!=NULL) free(sc);
    
}

