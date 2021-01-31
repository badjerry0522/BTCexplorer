#include <iostream>
#include <ostream>
#include <stdio.h>
#include "../include/core_type.h"
#include "../include/trans_file.h"
#include "../include/app_manager.h"
#include "../include/be.h"
#include "../include/tran_vec.h"


class tran_vec;
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
ERROR_CODE test_app(int app_argn,void **argv){
    test_tran_vec();
    return NO_ERROR;
}

struct app_record test_record={"test","NULL",test_app};