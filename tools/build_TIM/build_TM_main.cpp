#include<iostream>
#include"../../include/build_TIM.h"
#include"../../include/block_file.h"
#include"../../include/cache.h"
#include"../../include/trans_in_mem.h"
#include<sys/time.h>
#include<string.h>
#include <queue>
#include <fstream>
#include<stdio.h>
#include <stdlib.h>
#include<random>
using namespace std;
#define ADDR_STEP (1<<24)
struct addr2tran_t{
    queue<TRAN_SEQ> *trans;
    TRAN_SEQ last_tran;
};
struct frag_tran{
    TRAN_SEQ min_tran,max_tran;
};
void find_frag_tran(struct frag_tran *fp,trans_in_mem *tim,TRAN_SEQ max_tran){
    TRAN_SEQ tran;
    ADDR_SEQ *addr_list=(ADDR_SEQ *)malloc(sizeof(ADDR_SEQ)*(MAX_INPUT_NUM+MAX_OUTPUT_NUM));
    ERROR_CODE err;
    for(tran=0;tran<max_tran;tran++){
        int addr_num=tim->get_all_addr(tran,addr_list,&err);
        for(int i=0;i<addr_num;i++){
            ADDR_SEQ cur_addr=addr_list[i];
            int pos=cur_addr/ADDR_STEP;
            if(tran>fp[pos].max_tran) fp[pos].max_tran=tran;
            if(tran<fp[pos].min_tran) fp[pos].min_tran=tran;
        }
    }
    free(addr_list);
}
uint64_t build_addr2tran(ADDR_SEQ begin_addr,ADDR_SEQ end_addr,
    struct addr2tran_t *a2t,uint64_t *index,trans_in_mem *tim,
    TRAN_SEQ max_tran,uint64_t max_a2t,TRAN_SEQ _min_tran,TRAN_SEQ _max_tran){
    TRAN_SEQ tran;
    ERROR_CODE err;
    uint64_t _max_a2t;
    ADDR_SEQ *addr_list=(ADDR_SEQ *)malloc(sizeof(ADDR_SEQ)*(MAX_INPUT_NUM+MAX_OUTPUT_NUM));
    int t=0;
    for(tran=_min_tran;tran<=_max_tran;tran++){
        int addr_num=tim->get_all_addr(tran,addr_list,&err);
        for(int i=0;i<addr_num;i++){
            ADDR_SEQ cur_addr=addr_list[i];
            if((cur_addr<begin_addr)||(cur_addr>=end_addr)) continue;
            int pos=addr_list[i]-begin_addr;
            if(a2t[pos].last_tran!=tran){
                a2t[pos].last_tran=tran;
                a2t[pos].trans->push(tran);t++;
                //cout<<t<<" "<<cur_addr<<" "<<tran<<endl;
            }
        }
    }
    index[0]=a2t[0].trans->size()+max_a2t;
    ADDR_SEQ addr;
    for(addr=begin_addr+1;addr<end_addr;addr++){
        int pos=addr-begin_addr;
        _max_a2t=index[pos]=index[pos-1]+a2t[pos].trans->size();
    }
    
    free(addr_list);
    return _max_a2t;
}
void write_queue(FILE *handle,queue<TRAN_SEQ> *q){
    TRAN_SEQ temp[2048];
    int finished=0;
    int i;
    int qsize=q->size();
    while(qsize>=2048){
        for(i=0;i<2048;i++){
            temp[i]=q->front();
            q->pop();
        }
        fwrite(temp,sizeof(TRAN_SEQ),2048,handle);
        qsize-=2048;
    }
    for(i=0;i<qsize;i++){
        temp[i]=q->front();q->pop();
    }
    if(qsize!=0) fwrite(temp,sizeof(TRAN_SEQ),qsize,handle);
}
void write_addr2tran(ADDR_SEQ begin_addr,ADDR_SEQ end_addr,
    struct addr2tran_t *a2t,uint64_t *index, FILE *a2t_handle,FILE *index_handle){
    ADDR_SEQ addr;
    for(addr=begin_addr;addr<end_addr;addr++){
        int pos=addr-begin_addr;
        write_queue(a2t_handle,a2t[pos].trans);
    }
    fwrite(index,sizeof(uint64_t),end_addr-begin_addr,index_handle);
}
ERROR_CODE generate_addr2tran(char *dname,char *dest_dname){
    class trans_in_mem *tim=new trans_in_mem(dname);
    char fname[MAX_FNAME_SIZE];
    FILE *a2t_handle,*index_handle;
    strcpy(fname,dest_dname);
    strcat(fname,"addr2tran.dat");
    a2t_handle=fopen(fname,"wb");
    if(a2t_handle==NULL) return CANNOT_OPEN_FILE;

    strcpy(fname,dest_dname);
    strcat(fname,"addr2tran_index.dat");
    index_handle=fopen(fname,"wb");
    if(index_handle==NULL) return CANNOT_OPEN_FILE;
    
    ADDR_SEQ max_addr=tim->max_addr_seq();
    TRAN_SEQ max_tran=tim->max_tran_seq();
    int frag_num=(max_addr+ADDR_STEP-1)/ADDR_STEP;
    struct frag_tran *fp=(struct frag_tran *)malloc(sizeof(struct frag_tran)*frag_num);
    for(int i=0;i<frag_num;i++){
        fp[i].min_tran=NULL_SEQ;
        fp[i].max_tran=0;
    }
    cout<<"Finding frag of tran"<<endl;
    find_frag_tran(fp,tim,max_tran);

    struct addr2tran_t *a2t=(struct addr2tran_t *)malloc(sizeof(struct addr2tran_t)*ADDR_STEP);
    if(a2t==NULL) return OUT_OF_MEMORY;
    uint64_t *index=(uint64_t *)malloc(sizeof(uint64_t)*ADDR_STEP);
    if(index==NULL){
        free(a2t);
        return OUT_OF_MEMORY;
    }
    //cout<<"After malloc"<<endl;
    for(int i=0;i<ADDR_STEP;i++){
        a2t[i].last_tran=NULL_SEQ;
        a2t[i].trans=new queue<TRAN_SEQ>;
        index[i]=0;
    }
    cout<<"Init a2t and index"<<endl;
    uint64_t max_a2t=0;
    ADDR_SEQ begin_addr;
    //max_a2t=build_addr2tran(768382946,768382947,a2t,index,tim,max_tran,max_a2t);
    //return NO_ERROR;
    //max_addr=635184545;
    int cur_frag=0;
    //Searching for addresses in [begin_addr, begin_addr+ADDR_STEP)
    for(begin_addr=0;begin_addr<max_addr;begin_addr+=ADDR_STEP,cur_frag++){
        ADDR_SEQ end_addr=begin_addr+ADDR_STEP;
        
        if(end_addr>max_addr) end_addr=max_addr;
        max_a2t=build_addr2tran(begin_addr,end_addr,a2t,index,tim,max_tran,max_a2t,fp[cur_frag].min_tran,fp[cur_frag].max_tran);
        cout<<begin_addr<<"in "<<fp[cur_frag].min_tran<<" "<<fp[cur_frag].max_tran<<"max_a2t "<<max_a2t<<endl;
        write_addr2tran(begin_addr,end_addr,a2t,index,a2t_handle,index_handle);
        //Fresh the a2t and index
        for(int i=0;i<end_addr-begin_addr;i++){
            a2t[i].last_tran=NULL_SEQ;
            index[i]=0;
        }
    }
    free(a2t);
    fclose(a2t_handle);
    fclose(index_handle);

	strcpy(fname,dest_dname);
	strcat(fname,"a2t_meta.txt");
    cout<<fname<<endl;
	ofstream fin_meta(fname,ios::out);
	int version=0;
	fin_meta<<version<<" "<<max_addr<<" "<<max_a2t<<endl;
    fin_meta.close();
    return NO_ERROR;
}
ERROR_CODE generate_av(char *dir_name,char *output_dir_name){
	ERROR_CODE er=NO_ERROR;
    float time_use=0;
    struct timeval start,end;
    gettimeofday(&start,NULL);
    build_TIM(dir_name,0,output_dir_name,&er);
    gettimeofday(&end,NULL);
    time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);
    cout<<"time_use="<<time_use*1000<<"s"<<endl;
	return er;
}

int main(int argc,char **argv){
    cout<<"argc="<<argc<<endl;
    cout<<"dir_name="<<argv[2]<<endl;
    ERROR_CODE err;
    if(argc!=4){
        cout<<"Usage 0 srcdir destdir"<<endl;
        return 0;
    }
    if(strcmp(argv[1],"0")==0){
        err=generate_av(argv[2],argv[3]);
    }
    if(strcmp(argv[1],"1")==0){
        err=generate_addr2tran(argv[2],argv[3]);
    }
    return 1;
}