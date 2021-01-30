#include "../include/account.h"
#include "../include/core_type.h"

account::account(){
	seq=NULL_SEQ;
}
ACCOUNT_SEQ account::get_account_seq(){
	return seq;
}
void account::set_account_seq(ACCOUNT_SEQ seq1){
	seq=seq1;
}



