#ifndef _ADDRESS_QUERY_H
#define _ADDRESS_QUERY_H

#include "core_type.h"
#include "trans_in_mem.h"
#include <fstream>
#include <iostream>
using namespace std;

//addr_seq  --->  BTC_addr
ERROR_CODE search_addr_seq(ADDR_SEQ seq, char *destdname,char *btc_addr);

//BTC address ---> addr_seq
ADDR_SEQ search_btc(char *destdname,char *btc_addr,ERROR_CODE *err);

struct vol_time{
	CLOCK t;
	BTC_VOL vol;
};
struct address_info2{
	int tran_num;	
	BLOCK_SEQ first_block_seq,last_block_seq;
	CLOCK first_clock,last_clock;
	uint64_t balance;
};
struct address_info{
	ADDR_SEQ seq;
	char btc_address[MAX_BTC_ADDRESS_LEN];  		//seq---btc_address table
	ADDR_TYPE type;      
	struct address_info2 info2;	//seq--info2 table
	ACCOUNT_SEQ account_seq; 	//seq--account table
	int valid;
};
//Queue address information
//Only one object in system
class address_query{
	private:
		char dirname[MAX_FNAME_SIZE];
		char databaseconfig[MAX_FNAME_SIZE];     //Database Configuration File: host,port,user,password,database name.
		char address_info_fname[MAX_FNAME_SIZE]; //address info file
		char addr2account_fname[MAX_FNAME_SIZE]; //address to account file

	public:
		//Init with database name
		address_query(char *dir);
		//btc_addr --> addr_seq
		ADDR_SEQ get_addr_seq(char* btc_addr,ERROR_CODE *err);
		//addr_seq --> btc_addr
		ERROR_CODE get_btc_address(ADDR_SEQ seq,char *btc_addr);
		//output tran to ofstream
		void output_tran(ostream &os,struct transaction_binary *tp);
		//fill info with config
		void get_address_info(ADDR_SEQ seq,struct address_info *ai,int config,ERROR_CODE *err);
		//output the addr in os with config
		void output_addr(ADDR_SEQ seq,ostream os,int config);
		//output the addr info in the cout with config
		void output_addr(ADDR_SEQ seq,int config);

};
#endif