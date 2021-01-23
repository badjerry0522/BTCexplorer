#ifndef _ACCOUNT_SET_H
#define _ACCOUNT_SET_H
#include "core_type.h"
#include "account.h"
class account;
class account_set{
	public:
		account_set(char *database_name);
		
		int size();

		ACCOUNT_SEQ isIn(ADDR_SEQ seq); 
		~account_set();
};
#endif