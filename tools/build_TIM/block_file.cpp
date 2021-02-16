#include"../../include/core_type.h"
#include"../../include/trans_in_mem.h"
#include<cstring>
#include<cstdlib>
#include<stdlib.h>
#include<fstream>
#include<iostream>
using namespace std;
//ifstream fin("tx_34.txt",ios::in);
//turn (char*)number into uint64_t
uint64_t getvalue(char *buf,int *i,char signal_end){
    int start=*i;
    char temp[256];
    while(buf[*i]!=signal_end){ 
        (*i)++;
    }
    int end=(*i)-1;
    memcpy(temp,buf+start,end-start+1);
    //strncpy(temp,buf+start,(end-start+1));
    //cout<<"buf[end-start+1]="<<buf[end-start+1]<<endl;
    //cout<<"temp="<<temp<<endl;
    temp[end-start+1]='\0';
    uint64_t longres=atol(temp);
//    printf("getvalue: %llx from %s\n",longres,temp);
    
    //cout<<"longres="<<longres<<endl;
    return longres;
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
/*parse one transaction like:
{"blocktime":1342460748,"fee":50000,"inputs":[{"address":[4956718],"value":7190900}],"outputs":[{"address":[4960855],"value":140900},{"address":[3517040],"value":7000000}],"txhash":"c773685acb72987a4dc391c7f43bb8430475ed6eb68edff70e46b0d7cd2ef262"}
*/
ERROR_CODE parse_tran(char *buf,CLOCK *block_time,struct tran_info *tp, 
              ADDR_SEQ *addr, uint64_t *vol){
    enum state{readstart,GetBlockTime,GetFee,InputBegin,GetInputAV,OutputBegin,GetOutputAV,readend};//state of FSM
    state cur_state=readstart;
    int i=0;
    uint64_t v;
    tp->input_num=0;
    tp->output_num=0;
    while(buf[i]!='\0'){ 
        //cout<<"state="<<state<<endl;
        if(cur_state==readstart){
            i++;
            if(buf[i]=='\"'){
                i+=11;  //SKIP {"blocktime":
                cur_state=GetBlockTime;
            }
        }
        else if(cur_state==GetBlockTime){//get blocktime
            i++;
            int start=i;
            while(buf[i]!=',') i++;
            int end=i-1;
            char temp[100];
            strncpy(temp,buf+start,(end-start+1));
            *block_time=getblocktime(temp);
            cur_state=GetFee;
        }
        else if(cur_state==GetFee){//get fee
            i+=7;    //SKIP ,"fee":
            while(buf[i]!=',') i++;
            i+=2; //SKIP ,"
            cur_state=InputBegin;
        }
        else if(cur_state==InputBegin){//inputs begin
            i+=8; //SKIP ,"inputs":
            if(buf[i]=='['){
                cur_state=GetInputAV;
            }
        }
        else if(cur_state==GetInputAV){//get input addr and vol
            i+=12;  //SKIP {"address":
            uint32_t res[2];
            if(buf[i]=='['){//get addr
                i++;
                /*
                if(buf[i]=='-'){
                    i++;
                    //if(buf[i]==1) res[0]=0,res[1]=COINBASE_SEQ;
                    //else if(buf[i]==2) res[0]=0,res[1]=NonStandardAddress;
                }
                */

                v=getvalue(buf,&i,']');
                
                addr[tp->input_num]=(ADDR_SEQ) v;
                //cout<<"input addr "<<v<<"-->"<<addr[tp->input_num]<<endl;
                i+=10;  //SKIP ],"value":
                v=getvalue(buf,&i,'}');
                //cout<<"input vol "<<v<<endl;
                vol[tp->input_num]=v;
                tp->input_num++;
                i++;
                if(buf[i]==']') cur_state=OutputBegin;
                else cur_state=GetInputAV;
            }
        }
        else if(cur_state==OutputBegin){//outputs begin
            i+=12;  //SKIP }],"outputs":[{
            if(buf[i]=='[') cur_state=GetOutputAV;
        }
        else if(cur_state==GetOutputAV){//get output addr and vol
            i+=12;
            if(buf[i]=='['){
                i++;
                uint32_t res[2];
                v=getvalue(buf,&i,']');
                //cout<<"output addr="<<v<<endl;
                addr[tp->output_num+tp->input_num]=(ADDR_SEQ) v;
                i+=10;
                v=getvalue(buf,&i,'}');
                //cout<<"output vol="<<v<<endl;
                vol[tp->output_num+tp->input_num]=v;
                tp->output_num++;
                
                i++;
                if(buf[i]==']') cur_state=readend;
                else cur_state=GetOutputAV;
            }
        }
        else if(cur_state==readend) {
            break;
        }
    }
    return NO_ERROR;
    //printres(buf,block_time,tp,addr,vol);
}
