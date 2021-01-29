#include <iostream>
#include <ostream>
#include <stdio.h>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <stdlib.h>
#include <malloc.h>


#include "../include/core_type.h"
#include "../include/trans_file.h"
#include "../include/app_manager.h"
#include "../include/be.h"
using namespace std;

#define INPUT_TYPE 1
#define OUTPUT_TYPE 2

ERROR_CODE addr_of_tran_app(int app_argn,void **app_argv){
    if(app_argn!=3) return INVALID_ARG;
    char *trans_fname=(char *)app_argv[1];
    char *addr_fname=(char *)app_argv[2];
    char *type_str=(char *)app_argv[3];
    int type;
    if(strcmp(type_str,"input")==0){
        type=INPUT_TYPE;
    }
    else if(strcmp(type_str,"output")==0){
        type=OUTPUT_TYPE;
    }
    else if(strcmp(type_str,"all")==0){
        type=INPUT_TYPE+OUTPUT_TYPE;
    }
    else{
        return INVALID_ARG;
    }
    
    ERROR_CODE ret;
    
    trans_file *t=new trans_file();
    ret=t->open_trans_file(trans_fname);
    if(ret!=NO_ERROR)
        return ret;

    ofstream faddr;
    faddr.open(addr_fname,ios::out);
    if(!faddr.is_open()){
        delete t;
        return CANNOT_OPEN_FILE;
    }

    struct transaction *p=(struct transaction *)malloc(sizeof(struct transaction));
    set<string> *addr_set=new set<string>;
    t->begin();
    int trans=0;
    int need_input=type&INPUT_TYPE;
    int need_output=type&OUTPUT_TYPE;
    while(1){
        ret=t->next(p);
        if(ret==END_OF_FILE)
            break;
        if(need_input) add_addr2set(p->inputs,p->valid_inputs,addr_set);
        if(need_output) add_addr2set(p->outputs,p->valid_outputs,addr_set);
        //view_transaction(p);
        trans++;
    }
    cout<<addr_set->size()<<endl;
    output_addr_set(faddr,addr_set);
    faddr.close();
    free(p);
    return NO_ERROR;
}
struct app_record addr_of_tran_record={"addr_of_tran",
           "source_tran_file addr_file input|output|all",addr_of_tran_app};