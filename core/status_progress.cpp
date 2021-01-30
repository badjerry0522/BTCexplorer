#include <stdint.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <ostream>
#include "../include/status_progress.h"

status::status(char *fname){
    strcpy(file_name,fname);
}

void status::report(char *info){
    f.open(file_name,std::ofstream::out | std::ofstream::trunc);
    f<<info;
    f.close();
}

status::~status(){
}

progress::progress(char *fname,uint64_t _max){
    strcpy(file_name,fname);
    max=_max;
    gettimeofday(&last_time,NULL);
    last_value=0;
    ofstream f;
    f.open(file_name,std::ofstream::out | std::ofstream::trunc);
    f<<0;
    f.close();
}
void progress::value(uint64_t t){
    double r=(double)t/(double)max,intpart,fracpart;
    if(r>0.99){
        ofstream f;
        f.open(file_name,std::ofstream::out | std::ofstream::trunc);
        f<<100;
        f.close();
        return;
    }
    fracpart=modf(r*100,&intpart);
    int cur_value=(int)intpart;
    struct timeval cur_time;
    gettimeofday(&cur_time,NULL);
    float time_use=0;
    time_use=(cur_time.tv_sec-last_time.tv_sec)*1000000+(cur_time.tv_usec-last_time.tv_usec);
    if((cur_value>last_value)&&(time_use>10000)){ //>0.01s
        ofstream f;
        f.open(file_name,std::ofstream::out | std::ofstream::trunc);
        f<<cur_value;
        f.close();
        cout<<cur_value<<"%"<<endl;
        last_value=cur_value;
        memcpy(&last_time,&cur_time,sizeof(struct timeval));
    }
}

progress::~progress(){

}
