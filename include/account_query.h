#ifndef _ACCOUNT_QUERY_H
#define _ACCOUNT_QUERY_H
#include "../include/core_type.h"
#include "../include/account.h"
class account;
//Only ONE object in the system
class account_query{
    private:
		char account2addr_fname[MAX_FNAME_SIZE]; //account_info file
		char addr_list_fname[MAX_FNAME_SIZE];
		char account_info_fname[MAX_FNAME_SIZE];
		char addr2account_fname[MAX_FNAME_SIZE];
		ACCOUNT_SEQ max_account_seq;
		ADDR_SEQ max_addr_seq;

	public:
		account_query(char *account_dir);
		//addr_seq -->account_seq
		ACCOUNT_SEQ get_account_seq(ADDR_SEQ seq,ERROR_CODE *err);
		//addr_seq -->account object
		account *get_account_obj(ADDR_SEQ seq, ERROR_CODE *err);
		//addr_seq -->account information
		ERROR_CODE get_account_info(ACCOUNT_SEQ seq,struct account_info *info);
		//The number of accounts
		ACCOUNT_SEQ account_size();
		//The number of addresses
		ADDR_SEQ address_size();
		~account_query();
};
#endif