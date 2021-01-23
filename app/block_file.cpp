#include"../include/core_type.h"
#include"../include/trans_in_mem.h"
#include<cstring>
#include<cstdlib>
#include<stdlib.h>
#include<fstream>
#include<iostream>
using namespace std;
ifstream fin("tx_34.txt",ios::in);
void getvalue(char *buf,int *i,char signal_end,struct tran_info *tp,uint32_t *res){
    int start=*i;
    while(buf[*i]!=signal_end){ 
        (*i)++;
    }
    int end=(*i)-1;
    char *temp=(char *)malloc(sizeof(char )*(end-start+1));
    strncpy(temp,buf+start,(end-start+1));
    uint64_t longres=atol(temp);
    uint64_t one=1;
    if(longres>(one<<32)){
        tp->long_btc_vol=1;
        res[0]=(uint32_t)longres>>32;
        res[1]=(uint32_t)(((1<<32)-1)&longres);
    }
    else {
        res[0]=0;
        res[1]=(uint32_t) longres;
    }
}
uint64_t getblocktime(char *ch){
    //return _atoi64(ch);
    return atol(ch);
}
void check_length(char *buf,struct tran_info *tp,ADDR_SEQ *addr,uint32_t *vol){
    if(tp->long_btc_vol==1) return;
    for(int i=2;i<=tp->input_num*2;i+=2){
        addr[i/2]=addr[i];
        vol[i/2]=vol[i];
    }
    for(int i=(tp->input_num+1)*2;i<=(tp->input_num+tp->output_num)*2;i+=2){
        addr[i/2]=addr[i];
        vol[i/2]=vol[i];
    }
}
void printres(char *buf,CLOCK *block_time,struct tran_info *tp, 
              ADDR_SEQ *addr, uint32_t *vol){
        cout<<buf<<endl;
        cout<<"block_time="<<*block_time<<endl;
        cout<<"long?"<<" "<<tp->long_btc_vol<<endl;
        cout<<"input:"<<endl;
        if(tp->long_btc_vol==1){
            for(int i=1;i<=tp->input_num*2;i++) cout<<i<<":  "<<"addr:"<<addr[i]<<"   "<<"vol:"<<vol[i]<<endl;
            cout<<"output:"<<endl;
            for(int i=1;i<=tp->output_num*2;i++) cout<<i<<":  "<<"addr:"<<addr[tp->input_num*2+i]<<"   "<<"vol:"<<vol[tp->input_num*2+i]<<endl;
        }
        else{
            for(int i=1;i<=tp->input_num;i++) cout<<i<<":  "<<"addr:"<<addr[i]<<"   "<<"vol:"<<vol[i]<<endl;
            cout<<"output"<<endl;
            for(int i=1;i<=tp->output_num;i++) cout<<i<<":  "<<"addr:"<<addr[tp->input_num+i]<<"   "<<"vol:"<<vol[tp->input_num+i]<<endl;
        }
}
void parse_tran(char *buf,CLOCK *block_time,struct tran_info *tp, 
              ADDR_SEQ *addr, uint32_t *vol,ERROR_CODE *err){
    int state=0;
    int i=0;
    while(buf[i]!='\0'){
        if(state==0){
            i++;
            if(buf[i]=='\"'){
                state=1;
                i+=11;
                state=1;
            }
        }
        else if(state==1){//get blocktime
            i++;
            int start=i;
            while(buf[i]!=',') i++;
            int end=i-1;
            char *temp=(char *)malloc(sizeof(char)*(end-start+1));
            strncpy(temp,buf+start,(end-start+1));
            *block_time=getblocktime(temp);
            state=2;
        }
        else if(state==2){//get fee
            i+=7;
            while(buf[i]!=',') i++;
            i+=2;
            state=3;
        }
        else if(state==3){//inputs begin
            i+=8;
            if(buf[i]=='['){
                state=4;
            }
        }
        else if(state==4){//get input addr and vol
            tp->input_num++;
            i+=12;
            if(buf[i]=='['){//get addr
                i++;
                uint32_t *res=(uint32_t*)malloc(sizeof(uint32_t)*2);
                getvalue(buf,&i,']',tp,res);
                addr[(tp->input_num)*2-1]=res[0];
                addr[(tp->input_num)*2]=res[1];
                i+=10;
                getvalue(buf,&i,'}',tp,res);
                vol[(tp->input_num)*2-1]=res[0];
                vol[(tp->input_num)*2]=res[1];
                i++;
                if(buf[i]==']') state=5;
                else state=4;
            }
        }
        else if(state==5){//outputs begin
            i+=12;
            if(buf[i]=='[') state=6;
        }
        else if(state==6){//get output addr and vol
            tp->output_num++;
            i+=12;
            if(buf[i]=='['){
                i++;
                uint32_t *res=(uint32_t*)malloc(sizeof(uint32_t)*2);
                getvalue(buf,&i,']',tp,res);
                addr[(tp->input_num)*2+(tp->output_num)*2-1]=res[0];
                addr[(tp->input_num)*2+(tp->output_num)*2]=res[1];
                i+=10;
                getvalue(buf,&i,'}',tp,res);
                vol[(tp->output_num)*2+(tp->output_num)*2-1]=res[0];
                vol[(tp->input_num)*2+(tp->output_num)*2]=res[1];
                i++;
                if(buf[i]==']') state=7;
                else state=6;
            }
        }
        else if(state==7) {
            break;
        }
    }
    check_length(buf,tp,addr,vol);
    printres(buf,block_time,tp,addr,vol);
}
