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

class account_query;
class address_query;
class account;
ERROR_CODE account_of_addr_app(int app_argn,void **app_argv){
    if(app_argn!=2) return INVALID_ARG;
    BE_env *env=(BE_env *)app_argv[0];
    char *btc_addr=(char *)app_argv[1];
    char *addr_fname=(char *)app_argv[2];
    //cout<<"account_of_addr_app"<<btc_addr<<endl;
    address_query *addrq=env->addrq;
    account_query *accountq=env->accountq;

    ERROR_CODE err;
    ADDR_SEQ addr_seq=addrq->get_addr_seq(btc_addr,&err);
    //cout<<addr_seq<<endl;
    if(err!=NO_ERROR){
        return err;
    }
    ACCOUNT_SEQ acc_seq=accountq->get_account_seq(addr_seq,&err);
    cout<<"count_seq="<<acc_seq<<endl;
    if(err!=NO_ERROR) return err;

    account *a=accountq->get_account_obj(acc_seq,&err);
    if(err!=NO_ERROR) return err;
    cout<<"account size="<<a->get_size()<<endl;
    ofstream addr_fout;
    addr_fout.open(addr_fname,ios::out);
    int addr_num=0;
    char out_btc_addr[256];
    ADDR_SEQ cur_addr_seq=a->begin();
    while(cur_addr_seq!=NULL_SEQ){
        //cout<<"aadr_seq="<<cur_addr_seq<<endl;
         err=addrq->get_btc_address(cur_addr_seq,out_btc_addr);
         if(err!=NO_ERROR){
             cout<<"Error in get_btc_address"<<endl;
             break;
         }
         addr_num++;
         //cout<<addr_num<<endl;
         addr_fout<<out_btc_addr<<endl;
         cur_addr_seq=a->next();
    }
    addr_fout.close();
    delete a;
    return err;
}

struct app_record account_of_addr_record={"account_of_addr",
           "btc_addr addr_file",account_of_addr_app};