#include <fstream>
#include <iostream>
#include <map>
#include <set>
//#include <stream>
#include <string.h>
#include <stdio.h>
#include "../include/trans_file.h"

using namespace std;

void add_addr2set(struct addr_tran *p,int len,set<string> *addr_set){
    string s;
    for(int i=0;i<len;i++){
        s.assign(p[i].addr);
        set<string>::iterator iter;
        iter=addr_set->find(s);
        if(iter==addr_set->end())  addr_set->insert(s);
    }
}

void output_addr_set(ofstream &out,set<string> *addr_set){
	set<string>::iterator paddriter=addr_set->begin();
    while(paddriter!=addr_set->end()){
        out<<*paddriter<<endl;
    	paddriter++;
    }
}
int addr_in_set(struct addr_tran *p,int len,set<string> *addr_set,int and_flag){
	string s;
    set<string>::iterator iter;
    for(int i=0;i<len;i++){
        s.assign(p[i].addr);
        iter=addr_set->find(s);
        if((iter!=addr_set->end())&&(and_flag==0)){  //Find one match and and_flag==0
            return 1;
        }
		if((iter==addr_set->end())&&(and_flag!=0)){  //Find one unmatch and and_flag!=0
			return 0;
		}
		//One match and and_flag!=0 continue;
		//One unmatch and and_flag==0 continue;
    }
	//and_flag!=0 means all addr are matched
	//and_flag==0 means all addr are NOT matched
	if(and_flag!=0) return 1;
    return 0;
}
void view_transaction(struct transaction *t){
	cout<<t->tran_time<<" "<<t->seq<<endl;
	struct addr_tran *at=t->inputs;
	cout<<"inputs:"<<endl;
	for(int i=0;i<t->valid_inputs;i++){
		cout<<at[i].addr<<" "<<at[i].bitcoin<<endl;
	}
	cout<<"outputs:"<<endl;
	at=t->outputs;
	for(int i=0;i<t->valid_outputs;i++){
		cout<<at[i].addr<<" "<<at[i].bitcoin<<endl;
	}
	cout<<endl;
}

float btc_transaction(struct transaction *t){
	float sum=0.0;
	for(int i=0;i<t->valid_inputs;i++){
		sum+=t->inputs[i].bitcoin;
	}
	return sum;
}
trans_file::trans_file(){
	tran_num=0;
	pg=NULL;
}

ERROR_CODE trans_file::open_trans_file(char *filename){
	fin.open(filename,ios::in);
	if(!fin.is_open()){
		return CANNOT_OPEN_FILE;
	}
	return NO_ERROR;
}

ERROR_CODE trans_file::open_trans_file(char *filename,char *progress_fname){
	ERROR_CODE ret=open_trans_file(filename);
	if(ret!=NO_ERROR) return ret;
	fin.seekg(0, ios_base::end);
    uint64_t nFileLen = fin.tellg();
	cout<<"File size="<<nFileLen<<endl;
	pg=new progress(progress_fname,nFileLen);
	fin.seekg(0, ios_base::beg);
	return NO_ERROR;
}
ERROR_CODE trans_file::begin(){
	fin.seekg(0,ios::beg);
	state=TRAN_DATE;
	return NO_ERROR;
}


void set_addr_tran(struct addr_tran *at, char *addr,float bitcoin){
	strcpy(at->addr,addr);
	at->bitcoin=bitcoin;
}
void trans_file::addtran(char *buffer,struct transaction *t){
	t->seq=tran_num;
	tran_num++;
	strcpy(t->tran_time,buffer);
	t->valid_inputs=0;
	t->valid_outputs=0;
	//cout<<"add tran "<<buffer<<endl;
}
void trans_file::addinput(char *buffer,struct transaction *t){
	char addr[64];
	float bitcoin;
	sscanf(buffer,"%s %f",addr,&bitcoin);
	struct addr_tran *at=&(t->inputs[t->valid_inputs]);
	set_addr_tran(at,addr,bitcoin);
	t->valid_inputs++;
//	cout<<"add input "<<addr<<" "<<bitcoin<<endl;
}
void trans_file::addoutput(char *buffer,struct transaction *t){
	char addr[64];
	float bitcoin;
	sscanf(buffer,"%s %f",addr,&bitcoin);
	struct addr_tran *at=&(t->outputs[t->valid_outputs]);
	set_addr_tran(at,addr,bitcoin);
	t->valid_outputs++;
//	cout<<"add output "<<buffer<<endl;
}

STATE trans_file::process(char *buffer,STATE state,struct transaction *t){
	int l;
	l=strlen(buffer);
	if(buffer[l-1]==0xd){
		buffer[l-1]=0;
	}
	switch(state){
		case TRAN_DATE:
			addtran(buffer,t);
			state=INPUT;
			break;
		case INPUT:
			if(strcmp(buffer,"TO")==0){ //TO
				state=TOLINE;
			}
			else{
				addinput(buffer,t);
			}
			break;
		case TOLINE:
			addoutput(buffer,t);
			state=OUTPUT;
			break;
		case OUTPUT:
			if(strlen(buffer)==0){
				state=TRAN_DATE;
			}
			else{
				addoutput(buffer,t);
			}
			break;
		default:
			cout<<"Error STATE"<<endl;
	}
	return state;
}

ERROR_CODE trans_file::next(struct transaction *p){
	char buffer[256];
	while(!fin.eof()){
		fin.getline(buffer,256);
		state=process(buffer,state,p);
		if(pg!=NULL){
			uint64_t cur_pos=fin.tellg();
			//cout<<"cur_pos="<<cur_pos<<endl;
			pg->value(cur_pos);
		}
		if(state==TRAN_DATE)
			return NO_ERROR;
	}

	return END_OF_FILE;
}
int trans_file::get_tran_num(){
	return tran_num;
}
trans_file::~trans_file(){
	if(pg!=NULL) delete pg;
	fin.close();
}

output_trans_file::output_trans_file(){
	tran_num=0;
}
ERROR_CODE output_trans_file::open_trans_file(char *filename){
	fout.open(filename,ios::out);
	if(!fout.is_open()){
		return CANNOT_OPEN_FILE;
	}
	tran_num=0;
	return NO_ERROR;
}
ERROR_CODE  output_trans_file::append_tran(struct transaction *p){
	int i;
	fout<<p->tran_time<<endl;
	struct addr_tran *at=p->inputs;
	for(i=0;i<p->valid_inputs;i++){
		fout<<at[i].addr<<" "<<at[i].bitcoin<<endl;
	}
	fout<<"TO"<<endl;
	at=p->outputs;
	for(i=0;i<p->valid_outputs;i++){
		fout<<at[i].addr<<" "<<at[i].bitcoin<<endl;
	}
	fout<<endl;
	tran_num++;
	return NO_ERROR;
}
int  output_trans_file::get_tran_num(){
	return tran_num;
}
output_trans_file::~output_trans_file(){
	fout.close();
}

