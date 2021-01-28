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
int _addr_list_in_set(struct addr_tran *p,int len,set<string> *addr_set){
    string s;
    set<string>::iterator iter;
    for(int i=0;i<len;i++){
        s.assign(p[i].addr);
//        cout<<"s="<<s<<endl;
        iter=addr_set->find(s);
        if(iter!=addr_set->end()){
//            cout<<"s="<<s<<endl;
            return 1;
        }
    }

    return 0;
}
int _addr_in_set(struct transaction *p,set<string> *addr_set){
    int ret=_addr_list_in_set(p->inputs,p->valid_inputs,addr_set);
    if(ret==1) return 1;
    ret=_addr_list_in_set(p->outputs,p->valid_outputs,addr_set);
    return ret;
}
ERROR_CODE select_tran_app(int app_argn,void **app_argv){
    if(app_argn!=3) return INVALID_ARG;
    char *trans_fname=(char *)app_argv[1];
    char *addr_fname=(char *)app_argv[2];
    char *outtran_fname=(char *)app_argv[3];

    ERROR_CODE ret;
    
    trans_file *t=new trans_file();
    ret=t->open_trans_file(trans_fname);
    if(ret!=NO_ERROR)
        return ret;
    
    output_trans_file *d=new output_trans_file();
    ret=d->open_trans_file(outtran_fname);
    if(ret!=NO_ERROR){
        delete t;
        return ret;
    }
    
    set<string> addr_set;
    char buffer[256];
    ifstream faddr;
    faddr.open(addr_fname,ios::in);
    if(!faddr.is_open()){
        delete t;
        delete d;
        return CANNOT_OPEN_FILE;
    }
    
    string addr_str;
    while(!faddr.eof()){
        faddr>>addr_str;
//        faddr.getline(buffer,256);
//        addr_str.assign(buffer);
//        cout<<addr_str<<endl;
        addr_set.insert(addr_str);
    }
    faddr.close();
    cout<<"addr_set "<<addr_set.size()<<endl;
    
    struct transaction *p=(struct transaction *)malloc(sizeof(struct transaction));
    cout<<"Reading"<<endl;
    t->begin();
    int trans=0;
    int selected=0;
    while(1){
    //    cout<<trans<<" "<<selected<<endl;
        ret=t->next(p);
        if(ret==END_OF_FILE)
            break;
        int ret=_addr_in_set(p,&addr_set);
        if(ret==1){
            d->append_tran(p);
            selected++;
        }
       //view_transaction(p);
       //d->append_tran(p);
        trans++;
    }
    cout<<selected<<" of "<<trans<<endl;
    free(p);
    delete d;
    delete t;
    return NO_ERROR;

}
struct app_record select_tran_record={"select_tran",
           "source_tran_file addr_list_file result_tran_file",select_tran_app};