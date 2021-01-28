#include <fstream>
#include <iostream>
#include <map>
//#include <stream>
#include <string.h>
#include <stdio.h>
#include "../include/trans_file.h"

using namespace std;

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


trans_file::trans_file(){
	tran_num=0;
}

ERROR_CODE trans_file::open_trans_file(char *filename){
	fin.open(filename,ios::in);
	if(!fin.is_open()){
		return CANNOT_OPEN_FILE;
	}
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
		if(state==TRAN_DATE)
			return NO_ERROR;
	}
	return END_OF_FILE;
}
int trans_file::get_tran_num(){
	return tran_num;
}
trans_file::~trans_file(){
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

