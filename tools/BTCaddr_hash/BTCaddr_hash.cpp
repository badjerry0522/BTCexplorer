#include <fstream>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/core_type.h"
#define FRAGMENT_SIZE 32000
#define BTC_HASH_LEN 64
/* Example of input csv
0,1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa
1,12c6DSiU4Rq3P4ZxziKxzrL5LmMBrzjrJX
2,1HLoD9E4SDFFPDiYfNYnkBLQ85Y51J3Zb1
*/
/*file name ./data/addrseq_str.csv*/
using namespace std;


int sortBTC_addr(char *csvfname,char *outputfname,char *indexfname){
    char line[256];
    ifstream fin(csvfname,ios::in);

    if(!fin.is_open()){
        cout<<"Can not open file "<<csvfname<<endl;
        return 0;
    }
    map<string, string> *addr=new map<string,string>;
    string BTCstr,seqstr;
    int lines=1;
    while(!fin.eof()){
        char addr_seq[20];
        char BTCaddr[200];
        
        //Read addr_seq and BTCaddr
        char *token;
        fin.getline(line,255);
        if(strlen(line)==0) continue;
        token=strtok(line,",");
        strcpy(addr_seq,token);
        token=strtok(NULL," ");
        strcpy(BTCaddr,token);
        int btclen=strlen(BTCaddr);
        int seqlen=strlen(addr_seq);

        if((btclen>0)&&(seqlen>0)) {
            BTCstr.assign(BTCaddr);seqstr.assign(addr_seq);
            addr->insert(pair<string,string>(BTCstr,seqstr));
        }
        else{
            cout<<lines<<"Empty Line"<<endl;
        }

        if(lines%1000000==0){
            cout<<lines<<" "<<addr_seq<<" "<<BTCaddr<<endl;
        }
        lines++;
        //getchar();
    }
    fin.close();
    //cout<<addr->size()<<endl;

    map<string,string>::iterator iter;
    ofstream fout(outputfname,ios::out);
    ofstream indexout(indexfname,ios::out);

    if(!fout.is_open()){
        cout<<"Cannot open output file"<<endl;
        return 0;
    }
    if(!indexout.is_open()){
        cout<<"Cannot open index file"<<endl;
        return 0;
    }
    int begin_pos=0,cur_pos,frag;
    lines=1;
    iter=addr->begin();
    int begin_line=1;
    int frag_line;
    while(iter!=addr->end()){
        cur_pos=fout.tellp();
        frag=cur_pos-begin_pos;

        if(frag>FRAGMENT_SIZE){
            //cout<<lines<<endl;
            frag_line=lines-begin_line;
            indexout<<cur_pos<<","<<iter->first<<","<<frag_line<<endl;
            begin_pos=cur_pos;
            begin_line=lines;
        }
        fout<<iter->second<<","<<iter->first<<endl;
        lines++;
        iter++;
    }
    fout.close();
    indexout.close();
    delete addr;
    return 1;
}

int hash_src(char *srcdir,char *destdir){
    char srcfname[MAX_FNAME_SIZE];
    char destfname[MAX_FNAME_SIZE];
    ofstream fout[BTC_HASH_LEN];
    int lines_per_hash[BTC_HASH_LEN];
    int fsize_per_hash[BTC_HASH_LEN];
    char num[20];
    //Open hash files
    for(int i=0;i<BTC_HASH_LEN;i++){
        strcpy(destfname,destdir);
        strcat(destfname,"hash");
        sprintf(num,"%d.csv",i);
        strcat(destfname,num);
        fout[i].open(destfname,ios::out|ios::trunc);
        if(!fout[i].is_open()){
            cout<<destfname<<" cannot open"<<endl;
        }
        lines_per_hash[i]=0;
    }

    int filenum=0;
    while(1){
        //Open one SEQ--->btc_addr file
        filenum++;
        strcpy(srcfname,srcdir);
        strcat(srcfname,"seq_btc_");
        sprintf(num,"%d.csv",filenum);
        strcat(srcfname,num);
        ifstream fin(srcfname,ios::in);
        if(!fin.is_open()){
            break;
        }
        cout<<"Open "<<srcfname<<endl;
        char line[256];
        int lines=0;
        while(!fin.eof()){
            char addr_seq[20];
            char BTCaddr[200];
            
            //Read addr_seq and BTCaddr
            char *token;
            fin.getline(line,255);
            if(strlen(line)==0) continue;
            token=strtok(line,",");
            strcpy(addr_seq,token);
            token=strtok(NULL," ");
            strcpy(BTCaddr,token);
            int btclen=strlen(BTCaddr);
            int seqlen=strlen(addr_seq);

            if((btclen>0)&&(seqlen>0)) {
                int hash=btc_hash_6bit(BTCaddr);
                fout[hash]<<addr_seq<<","<<BTCaddr<<endl;
                lines_per_hash[hash]++;
            }
            else{
                cout<<lines<<"Empty Line"<<endl;
            }
            if(lines%1000000==0) cout<<lines<<" "<<addr_seq<<" "<<BTCaddr<<endl;
            
            lines++;
        }
        fin.close();
    }
    for(int i=0;i<BTC_HASH_LEN;i++){
        fsize_per_hash[i]=fout[i].tellp();
        fout[i].close();
    }
    int max_line=lines_per_hash[0],max_fsize=fsize_per_hash[0];
    int min_line=lines_per_hash[0],min_fsize=fsize_per_hash[0];
    
    for(int i=0;i<BTC_HASH_LEN;i++){
        if(lines_per_hash[i]>max_line) max_line=lines_per_hash[i];
        if(fsize_per_hash[i]>max_fsize) max_fsize=fsize_per_hash[i];
        if(lines_per_hash[i]<min_line) min_line=lines_per_hash[i];
        if(fsize_per_hash[i]<min_fsize) min_fsize=fsize_per_hash[i];
    
    }
    cout<<"     lines      fsize"<<endl;
    cout<<"Max  "<<max_line<<" "<<max_fsize<<endl;
    cout<<"Min  "<<min_line<<" "<<min_fsize<<endl;
    cout<<"     "<<(float)max_line/(float)min_line<<" "<<(float)max_fsize/(float)min_fsize<<endl;
    return filenum-1;
}
/*
Usage BTCaddr_hash src_dir dest_dir
Example:
    ./BTCaddr_hash ./data/seq2BTC/ ./data/BTC2seq/
*/
int main(int argn, char **argv){
    if(argn!=3) return 0;
    char *srcdname=(char *)argv[1];
    char *destdname=(char *)argv[2];

    char num[10];
    int ret;
    ret=hash_src(srcdname,destdname);
    cout<<"Hash "<<ret<<" files"<<endl;
    for(int i=0;i<BTC_HASH_LEN;i++){
        char csvfname[MAX_FNAME_SIZE];
        char outputfname[MAX_FNAME_SIZE];
        char indexfname[MAX_FNAME_SIZE];
        strcpy(csvfname,destdname);
        strcat(csvfname,"hash");
        sprintf(num,"%d.csv",i);
        strcat(csvfname,num);

        strcpy(outputfname,destdname);
        strcat(outputfname,"sorted");
        strcat(outputfname,num);

        strcpy(indexfname,destdname);
        strcat(indexfname,"index");
        strcat(indexfname,num);
        
        
        cout<<"sorting:"<<csvfname<<"-->"<<outputfname<<" "<<indexfname<<endl;
        ret=sortBTC_addr(csvfname,outputfname,indexfname);

        remove(csvfname);
    }
    return 0;
}