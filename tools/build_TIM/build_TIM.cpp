#include"../../include/trans_in_mem.h"
#include"../../include/build_TIM.h"
#include"../../include/core_type.h"
#include"../../include/block_file.h"
#include<fstream>
#include<cstdlib>
#include<stdlib.h>
#include<cstring>
#include<stdio.h>
#include<iostream>
#pragma pack(1)
using namespace std;
//open input file
uint64_t version=0,block_num=0,trans_num=0,addr_num=0,av_size=0,first_block_time=0,last_block_time=0;
fstream openfile(char *blockfiles_dir,int cur){
    char NO[4];
    sprintf(NO,"%d",cur);
    char filedes[80];
    strcpy(filedes,blockfiles_dir);
    strcat(filedes,"tx_");
    strcat(filedes,NO);
    strcat(filedes,".txt");
    fstream f1;
    f1.open(filedes,ios::in);
    return f1;
}

//print addr and vol to av.txt
void printav(ADDR_SEQ *addr,uint32_t *vol,struct tran_info *tp,FILE *fout_av){
    if(tp->long_btc_vol==1){
        //cout<<"sizeof(ADDR_SEQ)="<<sizeof(ADDR_SEQ)<<endl;
        fwrite(addr+1,sizeof(ADDR_SEQ),(tp->input_num+tp->output_num)*2,fout_av);
        fwrite(vol+1,sizeof(uint32_t),(tp->input_num+tp->output_num)*2,fout_av);
        av_size+=sizeof(uint32_t)*(tp->input_num+tp->output_num)*2*2;
    }
    else{
        fwrite(addr+1,sizeof(ADDR_SEQ),tp->input_num+tp->output_num,fout_av);
        fwrite(vol+1,sizeof(uint32_t),tp->input_num+tp->output_num,fout_av);
        av_size+=sizeof(uint32_t)*(tp->input_num+tp->output_num)*2;
    }
}

//print tran_info to trans.txt
void printtrans(struct tran_info *tp,FILE *fout_trans){
    trans_num++;
    if(trans_num==9111){
        cout<<"tp->index="<<tp->index<<endl;
        cout<<"tp->input_num="<<tp->input_num<<"  "<<"tp->output_num="<<tp->output_num<<endl;
    }
    //cout<<"sizeof struct tran_info="<<sizeof(tran_info)<<endl;
    fwrite(tp,sizeof(struct tran_info),1,fout_trans);
    //fwrite(tp,128,1,fout_trans);
}

//print block_info to block.txt
void printblock(struct block_info *bi,FILE *fout_block){
    //cout<<"sizeof struct block_info="<<sizeof(block_info)<<endl;
    fwrite(bi,sizeof(struct block_info),1,fout_block);
    //fwrite(bi,128,1,fout_block);
}
void get_max_addr(ADDR_SEQ *addr,ADDR_SEQ *max_addr,struct tran_info * ti){
    int len=ti->input_num+ti->output_num;
    //cout<<"ti->index="<<ti->index<<endl;
    //cout<<"len="<<len<<endl;
    //cout<<"old *max_addr="<<*max_addr<<endl;
    if(ti->long_btc_vol==0){
        for(int i=1;i<=len;i++)  if(addr[i]>*max_addr){
            if(addr[i]==COINBASE_SEQ||addr[i]==NonStandardAddress||addr[i]==OpReturn||addr[i]==NULL_SEQ) return;
            *max_addr=addr[i];
            //cout<<"*max_addr="<<*max_addr<<endl;
        }
    }
    else{
        for(int i=1;i<=len;i++) if(addr[i*2]>*max_addr){
            if(addr[i*2]==COINBASE_SEQ||addr[i*2]==NonStandardAddress||addr[i*2]==OpReturn||addr[i*2]==NULL_SEQ) return;
            *max_addr=(ADDR_SEQ )addr[i*2];
            
            //cout<<"*max_addr in 64="<<*max_addr<<endl;
        }
    }
}

//read in the first transaction
void init_first_trans(block_info *cur,fstream *fin,FILE *fout_av,
                      FILE *fout_trans,uint64_t *nxt_index,ADDR_SEQ *max_addr){
    char *buf=(char *)malloc(sizeof(char )*10000000);
    fin->getline(buf,10000000);
    CLOCK block_time;
    struct tran_info tp;
    tp.input_num=0,tp.output_num=0;tp.long_btc_vol=0;
    ADDR_SEQ addr[100000];
    uint32_t vol[100000];
    ERROR_CODE err;
    parse_tran(buf,&block_time,&tp,addr,vol,&err);
    tp.index=*nxt_index;
    printav(addr,vol,&tp,fout_av);
    get_max_addr(addr,max_addr,&tp);

    block_num++;
    cur->block_time=block_time;
    cur->first_tran_seq=0;
    cur->tran_num++;
    first_block_time=block_time;

    if(tp.long_btc_vol==1) *nxt_index+=(tp.input_num+tp.output_num)*2*2;
    else *nxt_index+=(tp.input_num+tp.output_num)*2;

    addr_num+=tp.input_num+tp.output_num;

    printtrans(&tp,fout_trans);
    free(buf);
}

//read in transinfo
void readtransinfo(char *buf,block_info *cur,uint64_t *nxt_index,
                   FILE *fout_trans,FILE *fout_block,FILE *fout_av,ADDR_SEQ *max_addr){
    CLOCK block_time;
    struct tran_info tp;
    tp.input_num=0,tp.output_num=0;tp.long_btc_vol=0;
    ADDR_SEQ addr[100000];
    uint32_t vol[100000];
    ERROR_CODE err;

    parse_tran(buf,&block_time,&tp,addr,vol,&err);
    get_max_addr(addr,max_addr,&tp);
    addr_num+=tp.input_num+tp.output_num;

    printav(addr,vol,&tp,fout_av);
    tp.index=*nxt_index;

    if(tp.long_btc_vol==1) *nxt_index+=(tp.input_num+tp.output_num)*2*2;
    else *nxt_index+=(tp.input_num+tp.output_num)*2;
    printtrans(&tp,fout_trans);
    if(block_time==cur->block_time){
        cur->tran_num++;
    }
    else{
        block_num++;
        last_block_time=block_time;
        printblock(cur,fout_block);
        cur->block_time=block_time;
        cur->first_tran_seq=cur->first_tran_seq+cur->tran_num;
        cur->tran_num=1;
    }
}
void creat_addr_show_times(FILE *fout_addr_show_times,uint64_t filesize){
    uint32_t temp[1];
    temp[0]=0;
    for(int i=0;i<filesize/4;i++) fwrite(temp,4,1,fout_addr_show_times);
}
void creat_addr_is_count(FILE *fout_addr_is_count,uint64_t filesize){
    uint32_t temp[1];
    temp[0]=0;
    for(int i=0;i<filesize/4;i++) fwrite(temp,4,1,fout_addr_is_count);
}
void build_TIM(char *blockfiles_dir, int first,char *output_dir,ERROR_CODE *err){
    int i=0;
    cout<<sizeof(struct tran_info)<<endl;
    fstream f1=openfile(blockfiles_dir,first);

    char des_av[100];//get output des of av
    strcpy(des_av,output_dir);
    strcat(des_av,"/av.txt");
    FILE *fout_av=fopen(des_av,"wb");

    char des_block[100];//get output des of block
    strcpy(des_block,output_dir);
    strcat(des_block,"/block.txt");
    FILE *fout_block=fopen(des_block,"wb");

    char des_trans[100];//get output des of trans
    strcpy(des_trans,output_dir);
    strcat(des_trans,"/trans.txt");
    FILE *fout_trans=fopen(des_trans,"wb");
    
    char des_addr_show_times[100];
    strcpy(des_addr_show_times,output_dir);
    strcat(des_addr_show_times,"/addr_show_times.txt");
    FILE *fout_addr_show_times=fopen(des_addr_show_times,"wb");

    char des_addr_is_count[100];
    strcpy(des_addr_is_count,output_dir);
    strcat(des_addr_is_count,"/addr_is_count.txt");
    FILE *fout_addr_is_count=fopen(des_addr_is_count,"wb");
    
    uint32_t max_addr=0;
    uint64_t nxt_index=0;
    block_info *cur;
    cur=(block_info *)malloc(sizeof(block_info));
    init_first_trans(cur,&f1,fout_av,fout_trans,&nxt_index,&max_addr);
    cout<<"init end"<<endl;

    while(f1.is_open()==true){
        cout<<"tx_"<<first+i<<"opened"<<endl;
        char *buf=(char *)malloc(sizeof(char )*1000000);
        while(!f1.eof()){
            f1.getline(buf,1000000);
            if(strlen(buf)==0) continue;
            readtransinfo(buf,cur,&nxt_index,fout_trans,fout_block,fout_av,&max_addr);
        }
        i++;

        free(buf);
        f1=openfile(blockfiles_dir,first+i);
    }
    fclose(fout_trans),fclose(fout_block),fclose(fout_av);
    free(cur);
    f1.close();
    char des_TIM_meta[100];
    strcpy(des_TIM_meta,output_dir);
    strcat(des_TIM_meta,"TIM_meta.txt");
    ofstream fout_TIM_meta(des_TIM_meta);
    fout_TIM_meta<<version<<" "<<block_num<<" "<<trans_num<<" "<<max_addr<<" "<<av_size<<" "<<" "<<first_block_time<<" "<<last_block_time<<" "<<endl;

    //creat_addr_show_times(fout_addr_show_times,(uint64_t)max_addr*4);
    //creat_addr_is_count(fout_addr_is_count,(uint64_t)max_addr*4);
}