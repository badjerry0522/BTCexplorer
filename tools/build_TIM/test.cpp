#include<iostream>
#include"../../include/build_TIM.h"
#include"../../include/block_file.h"
#include"../../include/cache.h"
#include"../../include/trans_in_mem.h"
#include<sys/time.h>
#include<string.h>
#include <fstream>
#include<stdio.h>
#include<random>
using namespace std;
int check_is_equal(unsigned char *p1,unsigned char *p2,int len){
    //cout<<"len="<<len<<endl;
    int ans=1;
    for(int i=0;i<len;i++){
        for(int j=0;j<8;j++){
            ans&=(p1[i]&1)==(p2[i]&1);
            p1[i]>>1,p2[i]>>1;
        }
    }
    return ans;
}
int randomtest(cache *ca){
    cout<<"randomtest begin"<<endl;
    int ans=1;
    default_random_engine e;
    struct tran_info ti;
    ti.index=100;
    ti.input_num=100;
    ti.long_btc_vol=1;
    ti.output_num=100;
    ti.unused=0;
    for(int i=0;i<5000000;i++){
        uint64_t index=e()%100000;
        //cout<<"index="<<index<<endl;
        unsigned char p[16];
        ERROR_CODE err;
        err=ca->store(index,(unsigned char *)&ti);
        //cout<<"store end"<<endl;
        ca->load(index,p);
        ans&=check_is_equal(p,(unsigned char *)&ti,16);
    }
    return ans;
}
int main(int argc,char **dir_name){
    cout<<"argc="<<argc<<endl;
    cout<<"dir_name="<<dir_name[2]<<endl;
    /*
    cache ca1;
    ERROR_CODE err;
    char filename[50]="trans.txt";
    err=ca1.init(filename,30,4,434721819);
    FILE *f1=fopen("p1.txt","wb");


    struct tran_info ti;
    ti.input_num=100;
    ti.output_num=10;
    ti.long_btc_vol=1;
    ti.unused=0;
    ti.index=10;


    unsigned char p[16];
    ca1.store(0,(unsigned char *)&ti);
    cout<<"store complete"<<endl;
    ca1.load(0,p);
    cout<<randomtest(&ca1)<<endl;
    if(f1!=NULL) fclose(f1);
    */


/*
    cout<<"hello"<<endl;
    ERROR_CODE er=NO_ERROR;
    float time_use=0;
    struct timeval start,end;
    gettimeofday(&start,NULL);
    build_TIM("/mnt/c/txdata/",0,"",&er);
    gettimeofday(&end,NULL);
    time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);
    cout<<"time_use="<<time_use*1000<<"s"<<endl;
    */

/*
    ifstream fin()
   	FILE *f1=fopen("addr_show_times.txt","wb");
   	FILE *f2=fopen("addr_is_count.txt","wb");
	if(f1==NULL) cout<<"times open failed"<<endl;
	if(f2==NULL) cout<<"count open failed"<<endl;
	creat_addr_show_times(f1,((uint64_t)764582874)*4);
	creat_addr_is_count(f2,((uint64_t)764582874)*4);
	cout<<"file creat complete"<<endl;
    fclose(f1),fclose(f2);
*/
	

   	//char dir_name[200]="/mnt/d/ubuntu/work/BTCexplorer/BTCexplorer/test";
    ERROR_CODE err;
    if(dir_name[1][0]=='1'){
        cout<<"type=1"<<endl;
   	    class trans_in_mem TIM(dir_name[2]);
    }
    else if(dir_name[1][0]=='0'){
        err=generate_av(dir_name[2],dir_name[3]);
    }
    
}