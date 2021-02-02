#include <string.h>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include "../include/statistics.h"

#define DEFAULT_GROUP 1
#define DEFAULT_LIMIT 31
using namespace std;
void statistics::_init(uint32_t  _groups,uint32_t  _limit,char *_output_fname){
    this->limit=_limit;
    this->groups=_groups;
    this->d=NULL;
    strcpy(this->fname,_output_fname);
    int size1=(limit+1)*groups;
    this->d=(int *)malloc(size1*sizeof(int));
    if(this->d==NULL){
        return;
    }
    for(int i=0;i<size1;i++) this->d[i]=0;
    this->title=NULL;
}
statistics::statistics(char *output_fname){
    _init(DEFAULT_GROUP,DEFAULT_LIMIT,output_fname);
}
statistics::statistics(uint32_t  limit,char *output_fname){
    _init(DEFAULT_GROUP,limit,output_fname);
}
statistics::statistics(uint32_t  _groups,uint32_t  _limit,char *_output_fname){
    _init(_groups,_limit,_output_fname);
}
void statistics::add(uint32_t group,uint32_t key,int value){
    if(group>=groups) return;
    if(d==NULL){
        return;
    }
    int *dg=&(d[group*(limit+1)]);
    if(key>=limit) dg[limit]=dg[limit]+value;
    else dg[key]=dg[key]+value;
}
void statistics::add(uint32_t key,int value){
    add(0,key,value);
}        
void statistics::output(){
    if(d==NULL){
        return;
    }
    int *p=d;
    int *sum=(int *)malloc(groups*sizeof(int));

    for(int g=0;g<groups;g++){
        sum[g]=0;
        for(int i=0;i<=limit;i++){
            sum[g]+=*p;p++;
        } 
    }
    ofstream fout(fname,ofstream::out);

    for(int i=0;i<=limit;i++){
        if(i==limit) fout<<">"<<limit;
        else fout<<i;

        fout<<"   ";
        for(int g=0;g<groups;g++){
            fout<<d[g*(limit+1)+i]<<"   ";
            double r=(double)d[g*(limit+1)+i]/(double)sum[g];
            fout<<r<<" ";
        }
        fout<<endl;
    }
    fout.close();
    free(sum);
}

statistics::~statistics(){
    if(d!=NULL)  free(d);
    if(title!=NULL) free(title);
}