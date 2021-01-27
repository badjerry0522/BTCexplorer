#include "account.h"


account::account(ACCOUNT_SEQ seq){
	this->seq = seq;
}

ACCOUNT_SEQ account::get_account_seq(){
	return this->seq;
}

ERROR_CODE account::get_account_info(struct account_info *info){
	this->account_info = info;
	return NO_ERROR;
}


