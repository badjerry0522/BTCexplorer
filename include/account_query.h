#ifndef _ACCOUNT_SET_H
#define _ACCOUNT_SET_H
#include "core_type.h"
#include "account.h"
class account;
//Only ONE object in the system
class account_query{
	public:
		account_query(char *database_name);
		
		ERROR_CODE get_account_info(ACCOUNT_SEQ seq,struct account_info *info);

		//The number of accounts
		int size();
		~account_query();
};
#endif