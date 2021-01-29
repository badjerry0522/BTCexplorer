#include <iostream>
#include <ostream>
#include <stdio.h>
#include <map>
#include <string>
#include <stdlib.h>
#include <malloc.h>


#include "../include/core_type.h"
#include "../include/trans_file.h"
#include "../include/app_manager.h"
#include "../include/be.h"
using namespace std;
struct info_trans{
    int tran_num;
    float btc_vol;
};

ERROR_CODE analyse_tran_app(int app_argn,void **app_argv){
    if(app_argn!=2) return INVALID_ARG;
    char *trans_fname=(char *)app_argv[1];
    char *dir_name=(char *)app_argv[2];
    char filename[256];
    strcpy(filename,dir_name);
    strcat(filename,"analyse_tran_month.txt");
    ofstream output;
    output.open(filename);

    ERROR_CODE ret;
    trans_file *t=new trans_file();
    ret=t->open_trans_file(trans_fname);
    if(ret!=NO_ERROR)
        return ret;
    map<string,struct info_trans> *data=new map<string,struct info_trans>;

    struct transaction *p=(struct transaction *)malloc(sizeof(struct transaction));
     t->begin();
     string monthstring;
     std::map<string,struct info_trans>::iterator iter;
     while(1){
        char month[32];
        ret=t->next(p);
        if(ret==END_OF_FILE)
            break;
        strcpy(month,p->tran_time);
        month[7]=0;
        monthstring.assign(month);
        float sum=btc_transaction(p);
        
        iter=data->find(monthstring);
        if(iter!=data->end()){
            iter->second.tran_num++;
            iter->second.btc_vol+=sum;
        }
        else{
            struct info_trans temp;
            temp.tran_num=1;temp.btc_vol=sum;
            data->insert(pair<string,struct info_trans>(monthstring,temp));
        }
    }
    cout<<data->size()<<endl;
    iter=data->begin();
    while(iter!=data->end()){
        output<<iter->first<<" "<<iter->second.tran_num<<" "<<iter->second.btc_vol<<endl;
        iter++;
    }

    output.close();
    free(data);
    free(p);
    delete t;
    return NO_ERROR;
}
struct app_record analyse_tran_record={"analyse_tran",
           "tran_file output_dir ",analyse_tran_app};