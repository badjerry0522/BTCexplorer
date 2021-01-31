#include "../include/address_query.h"

address_query::address_query(char *dir){
    
}

ADDR_SEQ address_query::get_addr_seq(string btc_addr,ERROR_CODE *err){
    return NULL_SEQ;
}

ERROR_CODE address_query::get_btc_address(ADDR_SEQ seq,char *btc_addr){
    return NO_ERROR;
}
		
