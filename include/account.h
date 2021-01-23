#ifndef _ACCOUNT_H
#define _ACCOUNT_H

#include "core_type.h"
#include "address_set.h"

class address_set;

class account:address_set{
	public:
		account();
		
		ACCOUNT_SEQ get_account_seq();	//获取账户序列号	
		~account();
};
#endif