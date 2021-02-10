#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/core_type.h"


using namespace std;


/*
Usage: search_hash destdir addr_seq
*/
#define FILE_LINES 20000000
#define SIZE_PER_LINE 50

int main(int argn, char **argv){
    if(argn!=3) return 0;
    char *destdname=(char *)argv[1];
    char *addrseq=(char *)argv[2];

    char datafname[MAX_FNAME_SIZE];
    char num[10];

    ADDR_SEQ  seq=atoi(addrseq);
    cout<<"seq="<<seq<<endl;
    int filenum=seq/FILE_LINES+1;
    strcpy(datafname,destdname);
    strcat(datafname,"seq_btc_");
    sprintf(num,"%d.csv",filenum);
    strcat(datafname,num);

    ifstream fin(datafname,ios::in);
    if(!fin.is_open()){
        cout<<"Can not find file:"<<datafname<<endl;
    }
    cout<<"Read file:"<<datafname<<endl;
    fin.seekg(0,ios::end);
    int max_pos=fin.tellg();
    //cout<<max_pos<<endl;

    int lines=seq%FILE_LINES;

    int cur_pos=lines*SIZE_PER_LINE;
    
    char line[256];
    char addr_seq[20];
    char BTCaddr[200];
    char *token;
    int found=0;
    int out_of_max_seq=0;
    while(1){
        int readnum;
        ADDR_SEQ dest_seq;
        int64_t delta;
        if(cur_pos>max_pos){
            cur_pos=max_pos-SIZE_PER_LINE*50;
        }
        if(cur_pos<0) cur_pos=0;

        cout<<"pos="<<cur_pos<<endl;
        
        fin.seekg(cur_pos,ios::beg);
        
        char c;    
        fin.read(&c,1);
        while(c!=0x0a){
            readnum=fin.readsome(&c,1);
            if(readnum==0) break;
        }
        

        if(readnum==0){
            cur_pos-=50*SIZE_PER_LINE;
            continue;
        }
        do{
            if(fin.eof()){
                out_of_max_seq=1;
                break;
            }
            fin.getline(line,255);
            if(strlen(line)==0) continue;
            token=strtok(line,",");
            strcpy(addr_seq,token);
            token=strtok(NULL," ");
            strcpy(BTCaddr,token);
            int btclen=strlen(BTCaddr);
            int seqlen=strlen(addr_seq);
            cout<<"seq="<<addr_seq<<endl;
            if(seqlen>0){
                dest_seq=atoi(addr_seq);
            }
            if(dest_seq==seq){
                cout<<BTCaddr<<endl;
                found=1;
                break;
            }
            
            delta=(uint64_t)seq-(uint64_t)dest_seq;
            //cout<<dest_seq<<" "<<delta<<endl;
            if((delta<0)||(delta>100)){
                cur_pos=fin.tellg();
                break;
            }
        }while(1);
        
        if((found==1)||(out_of_max_seq==1)) break;
        if(delta>=0){
            cur_pos+=delta*SIZE_PER_LINE;
        }
        else{  //delta<0
            delta=-delta;
            cur_pos-=delta*SIZE_PER_LINE*3;
        }
        //getchar();
    }
    fin.close();
    if(out_of_max_seq==1){
        cout<<"Out of max seq"<<endl;
    }
    return 0;
}