#ifndef _ADDRESS_SET_H
#define _ADDRESS_SET_H
#include <ostream>
#include <fstream>
#include <iostream>
#include <string>
#include "core_type.h"
#include "trans_in_mem.h"
#include "tran_set.h"

using namespace std;



struct address_set_info
{
	int firsttime;  //The earliest time
	int lasttime;   //The latest appearance time
	int K;          //Number of transactions
	int Kin;       	//Number of Input transactions
	int Kout;    	//Number of Ouput transactions
	LONG_BTC_VOL V;       //balance  LONG_BTC_VOL?
	LONG_BTC_VOL Vmax;  	//max balance
	LONG_BTC_VOL Vin;     //vol of all input transactions
	LONG_BTC_VOL Vout;    //vol of all output transactions
	bool Multisig; 	//is multi address
	int sumout;     //Number of accounts in input transactions
	int sumin;      //Number of accoutns in output transactions 
	int PROD;     	//Living time in seconds
	float PRKIKO;	//=Kin/Kout
	float TS;    	//=K/PROD
	bool VZERO;		//current balance is 0
	float VL;		//=(Vin+Vout)/Vmax
	int T10;//Within the time range of [T1, TK], the address amount is less than 10% of the maximum amount (unit: seconds)
	int T1050;//Within the time range of [T1, TK], the amount is 10% - 50% of the maximum amount (unit: seconds)
	int T50;//Within the time range of [T1, TK], the address amount is more than 50% of the maximum amount (unit: seconds)
	float PRT10;//The proportion of time when the address amount is less than 10% of the maximum amount
	float PRT1050;//The time proportion of address amount is 10% - 50% of the maximum amount
	float PRT50;//The proportion of time when the address amount is more than 50% of the maximum amount
	float ARANUM;//Average number of addresses involved in the transaction
	

	address_set_info() {
		firsttime = 0;
		lasttime = 0;
		K = 0;
		Kin = 0;
		Kout = 0;
		V = 0.0;
		Vmax = 0.0;
		Vin = 0.0;
		Vout = 0.0;
		Multisig = 0;  //false
		sumout = 0;
		sumin = 0;
		PROD = 0;
		PRKIKO = 0.0;
		TS = 0.0;
		VZERO = 0;
		VL = 0.0;
		T10 = 0;
		T1050 = 0;
		T50 = 0;
		PRT10 = 0.0;
		PRT1050 = 0.0;
		PRT50 = 0.0;
		ARANUM = 0.0;
	}
};


class tran_set;

class address_set {
private:
	//Init with NULL
	struct address_set_info *addr_set_info;
	ADDR_SEQ* addresses;
	int size;

public:
	//Empty Set
	address_set();
	//One address 
	address_set(string btc_address);
	address_set(ADDR_SEQ addr);

	//Multi addresses
	address_set(ADDR_SEQ *p, int num);
	address_set(char *filename);


	//Set and get labels
	void set_label(LABEL l);
	LABEL get_label(LABEL l);

	//Set and get names
	void set_name(string name);
	string get_name();

	int get_size();//The number of addresses in the address set

	//Add an address
	int push_back(string btc_address);
	int push_back(ADDR_SEQ addr);

	//judge whether the address is in this account
	int isIn(string btc_address);
	int isIn(ADDR_SEQ addr);

	ADDR_SEQ begin();
	ADDR_SEQ next();

	//The following call depends on the global transaction set
	CLOCK first_time();//The earliest time
	CLOCK last_time();//The latest appearance time
	uint64_t live_time(); //Latest appearance time - the earliest appearance time 

	BTC_VOL balance();//The final amount of bitcoins
	BTC_VOL balance(CLOCK t);//Number of bitcoins at time t	
	struct vol_time *balance_time();//The amount of bitcoin changes over time

	tran_set *get_trans(ERROR_CODE *err);

	//The following call depends on transaction set ts
	tran_set *trans(tran_set *ts);
	CLOCK first_time(tran_set *ts);
	CLOCK last_time(tran_set *ts);
	uint64_t live_time(tran_set *ts); //Latest appearance time - the earliest appearance time

	int output(ostream outs, int config);

	address_set operator+(const address_set &addr_set1);//Add two sets
	address_set operator-(const address_set &addr_set1);//Subtract two sets
	~address_set();
};
#endif