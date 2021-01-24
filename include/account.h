#ifndef _ACCOUNT_H
#define _ACCOUNT_H

#include "core_type.h"
#include "address_set.h"

class address_set;
struct account_info{
	
	//Other special information for account 
};
class account:address_set{
	private:
		ACCOUNT_SEQ seq;
		
		struct account_info *account_info;
	public:
		//create an account with an address seq and fill its account_info
		account(ACCOUNT_SEQ seq);
		
		ACCOUNT_SEQ get_account_seq();	//get acount seq
		//Fill the info struct
		ERROR_CODE get_account_info(struct account_info *info);	
		~account();
};
#endif