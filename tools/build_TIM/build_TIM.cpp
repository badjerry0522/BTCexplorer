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

void write_av(FILE *fout_av,struct tran_info *tp,ADDR_SEQ *addr,LONG_BTC_VOL *vol,BTC_VOL *short_vol){
    int sum=tp->input_num+tp->output_num;
    fwrite(addr,sizeof(ADDR_SEQ),sum,fout_av);
    if(tp->long_btc_vol==1){        
        fwrite(vol,sizeof(LONG_BTC_VOL),sum,fout_av);
    }
    else{
        for(int i=0;i<sum;i++){
            short_vol[i]=(BTC_VOL)vol[i];
        }
        fwrite(short_vol,sizeof(BTC_VOL),sum,fout_av);
    }
}

//print tran_info to trans.txt
void  write_tp(FILE *fout_trans,struct tran_info *tp){
    //cout<<"sizeof struct tran_info="<<sizeof(tran_info)<<endl;
    fwrite(tp,sizeof(struct tran_info),1,fout_trans);
    //fwrite(tp,128,1,fout_trans);
}

//print block_info to block.txt
void write_block(FILE *fout_block,struct block_info *bi){
    //cout<<"sizeof struct block_info="<<sizeof(block_info)<<endl;
    fwrite(bi,sizeof(struct block_info),1,fout_block);
    //fwrite(bi,128,1,fout_block);
}
uint16_t set_tran_long_btc_vol(int sum,LONG_BTC_VOL *vol){
    uint16_t islong=0;
    for(int i=0;i<sum;i++){
        if(vol[i]>=MIN_LONG_BTC_VOL){
            islong=1;
            break;
        }
    }
    return islong;
}
uint64_t build_tran_info(struct tran_info *tp,ADDR_SEQ *addr,
   LONG_BTC_VOL *vol,ADDR_SEQ *max_addr,uint64_t av_index,ERROR_CODE *err){
    uint64_t sum=tp->input_num+tp->output_num;
    if(sum==0){
        *err=INVALID_ARG;
        return av_index;
    }
    tp->long_btc_vol=set_tran_long_btc_vol(sum,vol);
    
    for(int i=0;i<sum;i++){
        if(addr[i]>0x80000000){
            *err=INVALID_ADDR_SEQ;
            return av_index;
        }
        if(addr[i]>*max_addr) *max_addr=addr[i];
    }
    uint64_t av_size_in_tran=sum+(sum<<tp->long_btc_vol);
    tp->index=av_index;
    *err=NO_ERROR;
    return av_index+av_size_in_tran;
}
void create_block(struct block_info *cur,CLOCK clock_time,int t_num,TRAN_SEQ t_seq){
    cur->block_time=clock_time;
    cur->tran_num=t_num;
    cur->first_tran_seq=t_seq;
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
    uint64_t version=0,block_num=0,trans_num=0,addr_num=0,first_block_time=0,last_block_time=0;

    cout<<sizeof(struct tran_info)<<endl;
    fstream f1=openfile(blockfiles_dir,first);

    char des_av[100];//get output des of av
    strcpy(des_av,output_dir);
    strcat(des_av,"av.dat");
    FILE *fout_av=fopen(des_av,"wb");

    char des_block[100];//get output des of block
    strcpy(des_block,output_dir);
    strcat(des_block,"block.dat");
    FILE *fout_block=fopen(des_block,"wb");

    char des_trans[100];//get output des of trans
    strcpy(des_trans,output_dir);
    strcat(des_trans,"trans.dat");
    FILE *fout_trans=fopen(des_trans,"wb");
    
    char des_addr_show_times[100];
    strcpy(des_addr_show_times,output_dir);
    strcat(des_addr_show_times,"addr_show_times.txt");
    FILE *fout_addr_show_times=fopen(des_addr_show_times,"wb");

    char des_addr_is_count[100];
    strcpy(des_addr_is_count,output_dir);
    strcat(des_addr_is_count,"addr_is_count.txt");
    FILE *fout_addr_is_count=fopen(des_addr_is_count,"wb");
    
    
    uint32_t max_addr=0;
    uint64_t av_index=0;
    struct block_info cur;
    struct tran_info tp;

    CLOCK clock_time;
    uint32_t sum_tran_num=0;

    int first_block=1;
    
    char *buf=(char *)malloc(sizeof(char )*1000000);
    ADDR_SEQ *addr=(ADDR_SEQ *)malloc(sizeof(ADDR_SEQ)*(MAX_INPUT_NUM+MAX_OUTPUT_NUM));
    LONG_BTC_VOL *vol=(LONG_BTC_VOL *)malloc(sizeof(LONG_BTC_VOL)*(MAX_INPUT_NUM+MAX_OUTPUT_NUM));
    BTC_VOL *short_vol=(BTC_VOL *)malloc(sizeof(BTC_VOL)*(MAX_INPUT_NUM+MAX_OUTPUT_NUM));
    
    while(f1.is_open()==true){
        //Read one file
        cout<<"Reading tx_"<<first+i<<endl;
        while(!f1.eof()){
            f1.getline(buf,1000000);
            if(strlen(buf)==0) continue;
            *err=parse_tran(buf,&clock_time,&tp,addr,vol);
            //cout<<clock_time<<" "<<tp.input_num<<" "<<tp.output_num<<endl;
            if(*err!=NO_ERROR){
                cout<<"Error in parse"<<endl; continue;
            }
            av_index=build_tran_info(&tp,addr,vol,&max_addr,av_index,err);
            if(*err==NO_ERROR){
                if(first_block==1){
                    first_block=0;
                    create_block(&cur,clock_time,1,0);
                    first_block_time=clock_time;
                }
                else{
                    if(clock_time==cur.block_time){
                        cur.tran_num++;
                    }
                    else{
                        sum_tran_num+=cur.tran_num;
                        write_block(fout_block,&cur);block_num++;
                        last_block_time=cur.block_time;
                        create_block(&cur,clock_time,1,sum_tran_num);
                    }
                }
                write_tp(fout_trans,&tp);
                //cout<<clock_time<<endl;
                //cout<<tp.input_num<<" "<<tp.output_num<<" "<<tp.long_btc_vol<<endl;
                trans_num++;
                write_av(fout_av,&tp,addr,vol,short_vol);
                /*
                int _index=0;
                for(int i=0;i<tp.input_num;i++){
                    cout<<addr[_index]<<" "<<vol[_index]<<endl;
                    _index++;
                }
                cout<<"TO"<<endl;
                for(int i=0;i<tp.output_num;i++){
                    cout<<addr[_index]<<" "<<vol[_index]<<endl;
                    _index++;
                }
                getchar();

                uint64_t cur_av_pos=ftell(fout_av);
                if(cur_av_pos!=(av_index*4)){
                    cout<<av_index<<" "<<cur_av_pos<<" "<<tp.input_num<<" "<<tp.output_num<<" "<<tp.long_btc_vol<<endl;
                    getchar();
                }
                */
            }
        }
        f1.close();
        i++;
        //Open Next file
        f1=openfile(blockfiles_dir,first+i);
    }

    //Last Block
    if(cur.tran_num!=0){
        write_block(fout_block,&cur);
        block_num++;
        last_block_time=cur.block_time;
    }
    free(buf);free(addr);free(vol);free(short_vol);
    fclose(fout_trans),fclose(fout_block),fclose(fout_av);
    f1.close();

    char des_TIM_meta[100];
    strcpy(des_TIM_meta,output_dir);
    strcat(des_TIM_meta,"TIM_meta.txt");
    ofstream fout_TIM_meta(des_TIM_meta);
    fout_TIM_meta<<version<<" "<<block_num<<" "<<trans_num<<" "<<max_addr<<" "<<av_index<<" "<<first_block_time<<" "<<last_block_time<<" "<<endl;

    //creat_addr_show_times(fout_addr_show_times,(uint64_t)max_addr*4);
    //creat_addr_is_count(fout_addr_is_count,(uint64_t)max_addr*4);
}
