#ifndef _ACCOUNT_H
#define _ACCOUNT_H

#include "core_type.h"
#include "address_set.h"

class address_set;
struct account_info{
	//int WANUM = 0; //number of addresses of the account
	int WMAXAPROD = 0;  //the longest active time of address in account
	float WAVGAPROD = 0.0; //average active time of address in account
	float WPMAXAPROD = 0.0; //= WMAXAPROD/PROD
	float WPAVGAPROD = 0.0; //= WAVGAPROD/PROD
	LONG_BTC_VOL WMAXAV = 0.0; //The maximum number of bitcoins for addresses in an account
	float WPMAXAV = 0.0; // = WMAXAV / VMAX
	//Other special information for account 
};
class account:address_set{
	private:
		ACCOUNT_SEQ seq;
		//struct account_info *account_info;
	public:
		//create an account with an address seq and fill its account_info
		account(ACCOUNT_SEQ seq);
		
		ACCOUNT_SEQ get_account_seq();	//get acount seq
		//Fill the info struct
		ERROR_CODE get_account_info(struct account_info *info);	
		~account();
};
#endif