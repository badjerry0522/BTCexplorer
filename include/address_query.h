#ifndef _ADDRESS_QUERY_H
#define _ADDRESS_QUERY_H

#include "../include/core_type.h"
#include <fstream>
#include <iostream>
using namespace std;

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
	public:
		//Init with database name
		address_query(char *database_name);
		//btc_addr --> addr_seq
		ADDR_SEQ get_addr_seq(string btc_addr,ERROR_CODE *err);
		//addr_seq --> btc_addr
		void get_btc_address(ADDR_SEQ seq,char *btc_addr,ERROR_CODE *err);
		//fill info with config
		void get_address_info(ADDR_SEQ seq,struct address_info *ai,int config,ERROR_CODE *err);
		//output the addr in os with config
		void output_addr(ADDR_SEQ seq,ostream os,int config);
		//output the addr info in the cout with config
		void output_addr(ADDR_SEQ seq,int config);
};
#endif