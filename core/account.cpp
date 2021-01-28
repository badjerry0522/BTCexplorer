#include "account.h"


account::account(ACCOUNT_SEQ seq){
	this->seq = seq;
	//Load the addr seqs in the account 
	//from ACCOUNT2ADDR and ADDR_LIST files into the aset

}

ACCOUNT_SEQ account::get_account_seq(){
	return this->seq;
}

ERROR_CODE account::get_account_info(struct account_info *info){
	//Read the account information from the ACCOUNT_INFO file to p
	//this->account_info = info;
	return NO_ERROR;
}


