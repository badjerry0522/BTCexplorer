#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/core_type.h"
#define FRAGMENT_SIZE 2000
/* Example of input csv
0,1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa
1,12c6DSiU4Rq3P4ZxziKxzrL5LmMBrzjrJX
2,1HLoD9E4SDFFPDiYfNYnkBLQ85Y51J3Zb1
*/
/*file name ./data/addrseq_str.csv*/
/*Test
4836094,111114KdW76em73CvERMrjDj9CU4eAEz2
9376600,111114S8436otvxakSnny9u1KavTTtgeF
2591544,11111Test111112222222222222LiApa
12434316,11111b9ZFr7agUkiueh1NKNvn1mhZHWu
2628431,11111i111111111111j1111111bL5SyF

*/

using namespace std;

struct sample_pos{
    uint64_t pos;
    int prev_frag_lines;
};

ADDR_SEQ searchBTC_addr(char *btc_addr,vector<string >*btc_v,vector<sample_pos> *pos_v,char *datafname){
    char line[256];
    ifstream fin(datafname,ios::in);

    if(!fin.is_open()){
        cout<<"Can not open file "<<datafname<<endl;
        return 0;
    }
    //Set BTC address string
    string btcstr;    
    btcstr.assign(btc_addr);

    std::vector<string>::iterator up;
    int data_pos;
    int data_line;
    int last_frag=0;
    struct sample_pos cur_pos;
    int index;
    //Find the upper_bound Of the index file
    up= std::upper_bound (btc_v->begin(), btc_v->end(), btcstr);
    if(up!=btc_v->end()){ 
        index=up-btc_v->begin();
        cur_pos=pos_v->at(index);
        //The lines of previous fragment
        data_line=cur_pos.prev_frag_lines;
        if(index>0){ //Read from the position of previous fragment
            cur_pos=pos_v->at(index-1);
            data_pos=cur_pos.pos;
        }
        else{//BTC_address < all ampled ones
            data_pos=0; //Read from the begin of data file
        }
    }
    else{//BTC_address >all sampled ones
        cur_pos=pos_v->at(pos_v->size()-1);
        //Read the position of the last fragment
        data_pos=cur_pos.pos;
        last_frag=1;
    }
    //cout<<"pos="<<data_pos<<" lines="<<data_line<<" "<<last_frag<<endl;

    //if last_frag==0 then Read data_line LINES from data_pos
    //                else read all lines from data_pos
    fin.seekg(data_pos,ios::beg);
    vector<string> fragbtc;
    vector<string> fragseq;
    string tempBTCstr,seqstr;
    int lines=0;
    while(!fin.eof()){
        char addr_seq[20];
        char BTCaddr[200];
        char *token;
        
        //Read one line to BTCaddr and addr_seq
        fin.getline(line,255);
        if(strlen(line)==0) continue;
        token=strtok(line,",");
        strcpy(addr_seq,token);
        token=strtok(NULL," ");
        strcpy(BTCaddr,token);
        int btclen=strlen(BTCaddr);
        int seqlen=strlen(addr_seq);

        //Push BTCaddr and addr_seq to th fragbtc and fragseq
        if((btclen>0)&&(seqlen>0)) {
//            cout<<BTCaddr<<endl;
            tempBTCstr.assign(BTCaddr);seqstr.assign(addr_seq);
            fragbtc.push_back(tempBTCstr);fragseq.push_back(seqstr);
            lines++;
        }
        if((last_frag==0)&&(lines==data_line)) break;
    }
    fin.close();

    //Find the upperbound of btcstr in fragbtc
    up=std::upper_bound(fragbtc.begin(),fragbtc.end(),btcstr);
    if(up!=fragbtc.end()){ //btc < special btc in fragbtc
        index=up-fragbtc.begin();
        if(index==0) return NULL_SEQ; //btc <the first btc in fragbtc
        index--;
    }
    else{ //btc >= the last btc
        index=fragbtc.size()-1; 
    }
    //Compare the btc before the upperbound with btcstr
    string destBTC=fragbtc.at(index);
    if(destBTC==btcstr){  //Match
        string destseq=fragseq.at(index);
        return atoi(destseq.c_str());
    }

    //Unmatch
    return NULL_SEQ;
}
/* index file:position, BTC_address, lines of previous fragment
868057172,3Qs7zYiydF8WYSsPVmHAMw8CgfPakEW7h5,46
868059196,3Qwvjt1NHocn7uiQndghrVdGTW9vhZakTg,46
*/
//Read index file
int read_index_file(char *indexfname,vector<string> *btc_v,vector<struct sample_pos> *pos_v){
    char line[256];
    ifstream fin(indexfname,ios::in);

    if(!fin.is_open()){
        cout<<"Can not open file "<<indexfname<<endl;
        return 0;
    }
    string BTCstr;

    while(!fin.eof()){
        char pos_str[20];
        char BTCaddr[200];
        char prev_line_str[20];
        char *token;
        struct sample_pos cur;

        //Read position BTC_address prev_frag lines
        fin.getline(line,255);
        if(strlen(line)==0) continue;
        token=strtok(line,",");
        strcpy(pos_str,token);
        token=strtok(NULL,",");
        strcpy(BTCaddr,token);
        token=strtok(NULL,",");
        strcpy(prev_line_str,token);

        //Add BTC_address to the vector btc_v, position and prev_frag_lines to pos_v
        BTCstr.assign(BTCaddr);
        cur.pos=atoi(pos_str);
        cur.prev_frag_lines=atoi(prev_line_str);

        btc_v->push_back(BTCstr);
        pos_v->push_back(cur);
    }
    return btc_v->size();
}
/*
Usage: search_hash destdir BTC_addr
*/
int main(int argn, char **argv){
    if(argn!=3) return 0;
    char *destdname=(char *)argv[1];
    char *btc_addr=(char *)argv[2];

    char datafname[MAX_FNAME_SIZE];
    char indexfname[MAX_FNAME_SIZE];
    char num[10];
    int hash=btc_hash_6bit(btc_addr);
    strcpy(datafname,destdname);
    strcat(datafname,"sorted");
    sprintf(num,"%d.csv",hash);
    strcat(datafname,num);

    strcpy(indexfname,destdname);
    strcat(indexfname,"index");
    strcat(indexfname,num);
    
    vector<string> *btc_sample_v=new vector <string>;
    vector<struct sample_pos> *pos_sample_v=new vector<struct sample_pos>;
    int ret;
    ret=read_index_file(indexfname,btc_sample_v,pos_sample_v);


    ADDR_SEQ addr;
    addr=searchBTC_addr(btc_addr,btc_sample_v,pos_sample_v,datafname);
    if(addr==NULL_SEQ){
        cout<<"Not Found"<<endl;
    }
    else{
        cout<<addr<<endl;
    }
    return 0;
}