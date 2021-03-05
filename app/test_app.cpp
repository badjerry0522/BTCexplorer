#include <iostream>
#include <ostream>
#include <stdio.h>
#include "../include/core_type.h"
#include "../include/trans_file.h"
#include "../include/app_manager.h"
#include "../include/be.h"
#include "../include/tran_vec.h"
#include "../include/statistics.h"
#include "../include/trans_in_mem.h"

class tran_vec;
class statistics;

void test_tran_vec(){
    cout<<"Test tran_vec"<<endl;
    tran_vec *tv=new tran_vec();
 //   cout<<"Push back"<<endl;
    tv->push_back(0);
    tv->push_back(1);
    tv->push_back(2);
    tv->push_back(3);
    tv->push_back(4);
//  cout<<tv->size()<<endl;
    TRAN_SEQ seq,seq0;
    seq=tv->begin();
    while(seq!=NULL_SEQ){
        cout<<seq<<endl;
        seq=tv->next();
    }
    cout<<"------"<<endl;
    seq=tv->begin(LAST);
    while(seq!=NULL_SEQ){
        cout<<seq<<endl;
        seq=tv->next();
    }
    cout<<"------"<<endl;

    for(seq0=0;seq0<=6;seq0++){
        cout<<"seq0="<<seq0<<endl;
        seq=tv->begin(seq0,BEFORE,LAST);
        while(seq!=NULL_SEQ){
            cout<<seq<<endl;
            seq=tv->next();
        }
        cout<<"------"<<endl;
    }

    
    delete tv;
}
void test_CLOCK(){
    char text_time[20];
    CLOCK t;
    CLOCK2string(1512161692,text_time);
    cout<<text_time<<endl;
    t=string2CLOCK(text_time);
    cout<<t<<endl;

}
void test_statistics(){
    statistics *s1=new(nothrow) statistics(2,10,"test_stat.txt");
    /*
    statistics *s=(statistics*)::operator new(sizeof(statistics));
    
    cout<<"new complete"<<endl;
    cout<<"s="<<s<<endl;
    new(s) statistics(10,"test_stat.txt");
    */
    s1->add(0,1,10);
    s1->add(1,1,10);
    s1->add(1,100,2);
    s1->add(0,0,5);
    s1->add(0,0,10);
    s1->output();

    delete s1;
}
void test_TIM(TRAN_SEQ seq,address_query *addrq,trans_in_mem *tim){
    struct transaction_binary *tp=(struct transaction_binary *)malloc(sizeof(struct transaction_binary));
    tim->get_tran_binary(seq,tp);
    //cout<<"After get_tran_binary"<<endl;
    
    addrq->output_tran(cout,tp);

    free(tp);
}
void test_addr2tran(char *btc_addr,address_query *addrq,trans_in_mem *tim,addr2tran *a2t){
    ERROR_CODE err;
    ADDR_SEQ addr=addrq->get_addr_seq(btc_addr,&err);
    cout<<"addr2tran: addr="<<addr<<endl;
    tran_vec *tran;
    tran=a2t->get_tran_set(addr,&err);
    if(err!=NO_ERROR){
        cout<<"Error for get_tran_set():"<<error_string(err)<<endl;
        return;
    }
    cout<<tran->size()<<endl;
    TRAN_SEQ t;
    struct transaction_binary *tp=(struct transaction_binary *)malloc(sizeof(struct transaction_binary));
    
    t=tran->begin();
    while(t!=NULL_SEQ){
        tim->get_tran_binary(t,tp);
    //cout<<"After get_tran_binary"<<endl;
    
        addrq->output_tran(cout,tp);
        t=tran->next();
    }
    free(tp);
    delete tran;
}
ERROR_CODE test_app(int app_argn,void **argv){
    //test_tran_vec();
    //test_CLOCK();
    //test_statistics();
    BE_env *env=(BE_env *)argv[0];
    char *btc_addr=(char *)argv[1];
    address_query *addrq=env->addrq;
    trans_in_mem *tim=env->tim;
    addr2tran *a2t=env->a2t;
    //cout<<seq<<endl;
    //test_TIM(seq,addrq,tim);
    test_addr2tran(btc_addr,addrq,tim,a2t);
    return NO_ERROR;
}

struct app_record test_record={"test","NULL",test_app};