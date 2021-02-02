#include"../include/core_type.h"
#include"../include/trans_in_mem.h"
#include<cstring>
#include<cstdlib>
#include<stdlib.h>
#include<fstream>
#include<iostream>
using namespace std;
ifstream fin("tx_34.txt",ios::in);
void getvalue(char *buf,int *i,char signal_end,struct tran_info *tp,uint32_t *res){//turn (char)number into (uint32_t)number
    int start=*i;
    char temp[256];
    while(buf[*i]!=signal_end){ 
        (*i)++;
    }
    int end=(*i)-1;
    strncpy(temp,buf+start,(end-start+1));
    uint64_t longres=atol(temp);
    uint64_t one=1;

    //special address
    if(buf[start]=='-'){
        if(buf[start+1]=='1'){ longres=COINBASE_SEQ;}//cout<<"addr=-1"<<endl;}
        else if(buf[start+1]=='2') {longres=NonStandardAddress;}//cout<<"addr=-1"<<endl;}
        else if(buf[start+1]=='3') {longres=OpReturn;}//cout<<"addr=-3"<<endl;}
    }
    if(start==end) longres=NULL_SEQ;


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
    return atol(ch);
}
void check_length(char *buf,struct tran_info *tp,ADDR_SEQ *addr,uint32_t *vol){//check for long btc vol 
    if(tp->long_btc_vol==1) return;
    for(int i=2;i<=tp->input_num*2;i+=2){//if long_btc_vol==0 store addr and vol in uint_32t
        addr[i/2]=addr[i];
        vol[i/2]=vol[i];
    }
    for(int i=(tp->input_num+1)*2;i<=(tp->input_num+tp->output_num)*2;i+=2){
        addr[i/2]=addr[i];
        vol[i/2]=vol[i];
    }
}
void printres(char *buf,CLOCK *block_time,struct tran_info *tp, //print tp
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
    enum state{readstart,GetBlockTime,GetFee,InputBegin,GetInputAV,OutputBegin,GetOutputAV,readend};//state of FSM
    state cur_state=readstart;
    int i=0;
    while(buf[i]!='\0'){
        //cout<<"state="<<state<<endl;
        if(cur_state==readstart){
            i++;
            if(buf[i]=='\"'){
                i+=11;
                cur_state=GetBlockTime;
            }
        }
        else if(cur_state==GetBlockTime){//get blocktime
            i++;
            int start=i;
            while(buf[i]!=',') i++;
            int end=i-1;
            char *temp=(char *)malloc(sizeof(char)*(end-start+1));
            strncpy(temp,buf+start,(end-start+1));
            *block_time=getblocktime(temp);
            free(temp);
            cur_state=GetFee;
        }
        else if(cur_state==GetFee){//get fee
            i+=7;
            while(buf[i]!=',') i++;
            i+=2;
            cur_state=InputBegin;
        }
        else if(cur_state==InputBegin){//inputs begin
            i+=8;
            if(buf[i]=='['){
                cur_state=GetInputAV;
            }
        }
        else if(cur_state==GetInputAV){//get input addr and vol
            tp->input_num++;
            i+=12;
            if(buf[i]=='['){//get addr
                i++;
                
                uint32_t res[2];
                getvalue(buf,&i,']',tp,res);
                addr[(tp->input_num)*2-1]=res[0];
                addr[(tp->input_num)*2]=res[1];
                i+=10;
                getvalue(buf,&i,'}',tp,res);
                vol[(tp->input_num)*2-1]=res[0];
                vol[(tp->input_num)*2]=res[1];
                i++;
                if(buf[i]==']') cur_state=OutputBegin;
                else cur_state=GetInputAV;
            }
        }
        else if(cur_state==OutputBegin){//outputs begin
            i+=12;
            if(buf[i]=='[') cur_state=GetOutputAV;
        }
        else if(cur_state==GetOutputAV){//get output addr and vol
            tp->output_num++; 
            i+=12;
            if(buf[i]=='['){
                i++;
                uint32_t res[2];
                getvalue(buf,&i,']',tp,res);
                addr[(tp->input_num)*2+(tp->output_num)*2-1]=res[0];
                addr[(tp->input_num)*2+(tp->output_num)*2]=res[1];
                i+=10;
                getvalue(buf,&i,'}',tp,res);
                vol[(tp->output_num)*2+(tp->output_num)*2-1]=res[0];
                vol[(tp->input_num)*2+(tp->output_num)*2]=res[1];
                i++;
                if(buf[i]==']') cur_state=readend;
                else cur_state=GetOutputAV;
            }
        }
        else if(cur_state==readend) {
            break;
        }
    }
    check_length(buf,tp,addr,vol);
    //printres(buf,block_time,tp,addr,vol);
}
