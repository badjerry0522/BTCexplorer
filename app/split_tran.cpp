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
struct _split_tran_set{
    set<string> *addr;  //addresses in transactions
    vector<int> *tran;  //transaction group
    set<string> *paddr;  //public address 
};

//Add q to p
void combine(struct _split_tran_set *p,struct _split_tran_set *q){
    //cout<<"combine p"<<p->addr->size()<<" "<<p->paddr->size()<<" "<<p->tran->size()<<endl;
    //cout<<"combine q"<<q->addr->size()<<" "<<q->paddr->size()<<" "<<q->tran->size()<<endl;

    p->addr->insert(q->addr->begin(),q->addr->end());
    p->tran->insert(p->tran->end(),q->tran->begin(),q->tran->end());
    p->paddr->insert(q->paddr->begin(),q->paddr->end());
    //cout<<"after combine p "<<p->addr->size()<<" "<<p->paddr->size()<<" "<<p->tran->size()<<endl;
    //cout<<"after combine q "<<q->addr->size()<<" "<<q->paddr->size()<<" "<<q->tran->size()<<endl;
}
void add_paddr_set(struct addr_tran *p,int len, struct _split_tran_set  *homes){
    string s;
    set<string>::iterator iter;
    for(int i=0;i<len;i++){
        s.assign(p[i].addr);
        iter=homes->addr->find(s);
        //found the public address
        if(iter!=homes->addr->end()){
 //           cout<<s<<endl;
            iter=homes->paddr->find(s);
            if(iter==homes->paddr->end()){
                homes->paddr->insert(s);
//                cout<<"homes paddr size="<<homes->paddr->size()<<" "<<s<<endl;
            }
        }
    }
}
int cmp_addr_list(struct addr_tran *p,int len, set<string> *addrp){
    string s;
    set<string>::iterator iter;
    for(int i=0;i<len;i++){
        s.assign(p[i].addr);
        iter=addrp->find(s);
        if(iter!=addrp->end()){
            return 0;
        }
    }
    return -1;
}
int cmp_addr(struct transaction *p,struct _split_tran_set *q){
    int ret;
    ret=cmp_addr_list(p->inputs,p->valid_inputs,q->addr);
    if(ret==0) return 0;
    ret=cmp_addr_list(p->outputs,p->valid_outputs,q->addr);
    return ret;
}
void add_addr_set(struct addr_tran *p,int len,set<string> *addrp){
    string s;
    for(int i=0;i<len;i++){
        s.assign(p[i].addr);
        set<string>::iterator iter;
        iter=addrp->find(s);
        if(iter==addrp->end())  addrp->insert(s);
    }
}
void add_tran(struct transaction *p,struct _split_tran_set *s){
    s->tran->push_back(p->seq);
    add_paddr_set(p->inputs,p->valid_inputs,s);
    add_paddr_set(p->outputs,p->valid_outputs,s);
//    cout<<"add public addr end"<<endl;
    add_addr_set(p->inputs,p->valid_inputs,s->addr);
//    cout<<"add inputs to home"<<endl;
    add_addr_set(p->outputs,p->valid_outputs,s->addr);
//    cout<<"add outputs to home"<<endl;
}
void push_list(struct transaction *p,list<struct _split_tran_set> *gp){
    struct _split_tran_set s;
    s.addr=new set<string>;
    s.paddr=new set<string>;
    s.tran=new vector<int>;
    add_addr_set(p->inputs,p->valid_inputs,s.addr);
    add_addr_set(p->outputs,p->valid_outputs,s.addr);
    s.tran->push_back(p->seq);
    gp->push_back(s);
}
void release_node(struct _split_tran_set *p){
    delete p->addr;
    delete p->tran;
    delete p->paddr;
}

void _split_add_tran(struct transaction *p,list<struct _split_tran_set> *gp){
    if(gp->size()==0){
        push_list(p,gp);
    }
    list<struct _split_tran_set>::iterator iter;
    int found=0;
    iter=gp->begin();
    
    while(iter!=gp->end()){
        struct _split_tran_set *firstfind,*cur;
        cur=&(*iter);
        if(cmp_addr(p,cur)==0){
            if(found==0){
                found=1;
                add_tran(p,cur);
                firstfind=cur;
                iter++;
            }
            else{
                combine(firstfind,cur);
                release_node(cur);              
                gp->erase(iter++);
            }
        }
        else{
            iter++;
        }
    }
    if(found==0) push_list(p,gp);
}
ERROR_CODE split_tran_app(int app_argn,void **app_argv){
    if(app_argn!=2){
        return INVALID_ARG;
    }
    list<struct _split_tran_set> *groups=new list<struct _split_tran_set>;
    struct BE_env *ev=(struct BE_env *)app_argv[0];
    char *trans_fname=(char *)app_argv[1];
    char *result_dname=(char *)app_argv[2];
    ERROR_CODE ret;

    trans_file *t=new trans_file();
    ret=t->open_trans_file(trans_fname);
    if(ret!=NO_ERROR)
        return ret;
    struct transaction *p=(struct transaction *)malloc(sizeof(struct transaction));

    t->begin();
    int trans=0;
    while(1){
        ret=t->next(p);
        if(ret==END_OF_FILE)
            break;
        
        //view_transaction(p);
        trans++;
        _split_add_tran(p,groups);
        cout<<trans<<" "<<groups->size()<<endl;
    }
    cout<<groups->size()<<endl;
    char paddrfname[256];

    strcpy(paddrfname,(char *)app_argv[2]);
    strcat(paddrfname,"paddrs.txt");
    ofstream paddrfile;
    paddrfile.open(paddrfname);

    int set1count=0;
    list<struct _split_tran_set>::iterator iter;
    iter=groups->begin();
    while(iter!=groups->end()){
        if(iter->tran->size()>1){
            cout<<iter->tran->size()<<endl;
            paddrfile<<iter->addr->size()<<" "<<iter->tran->size()<<" "<<iter->paddr->size()<<endl;
            set<string>::iterator paddriter=iter->paddr->begin();
            
            while(paddriter!=iter->paddr->end()){
                paddrfile<<*paddriter<<endl;
                paddriter++;
            }
            
            paddrfile<<"--------------"<<endl;

            set1count++;
        }
        iter++;
    }

    iter=groups->begin();
    while(iter!=groups->end()){
        struct _split_tran_set *cur=&(*iter);
        release_node(cur);
        iter++;
    }
    delete groups;
    malloc_trim(0);
    paddrfile.close();
    
    cout<<set1count<<endl;
    return NO_ERROR;

}

struct app_record split_tran_record={"split_tran","source_file result_dir",split_tran_app};