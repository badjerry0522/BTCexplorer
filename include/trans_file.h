#ifndef _TRANFILE_H
#define _TRANFILE_H
#include <fstream>
#include <iostream>
#include <map>
//#include <stream>
#include <string.h>
#include <stdio.h>
#include "core_type.h" 

using namespace std;

enum STATE{TRAN_DATE,INPUT,TOLINE,OUTPUT};
struct addr_tran{
	char addr[64];
	float bitcoin;
};
struct transaction{
	int seq;
	char tran_time[32];
	struct addr_tran inputs[MAX_INPUT_NUM];
	struct addr_tran outputs[MAX_OUTPUT_NUM];
	int valid_inputs;
	int valid_outputs;
};
void view_transaction(struct transaction *t);
class trans_file{
	private:

		int tran_num;
		ifstream fin;
		STATE state;
		void addtran(char *buffer,struct transaction *t);
		void addinput(char *buffer,struct transaction *t);
		void addoutput(char *buffer,struct transaction *t);
		STATE process(char *buffer,STATE state,struct transaction *t);

	public:
		trans_file();
		ERROR_CODE open_trans_file(char *filename);

		ERROR_CODE begin();
		ERROR_CODE next(struct transaction *p);
		
		int get_tran_num();
		~trans_file();
};
#endif