#include<iostream>
#include "../include/account.h"
#include "../include/address_set.h"
#include "../include/account_query.h"
#include "../include/address_query.h"
int main(int argc, char *argv[])
{
	//directory
	char* dir = (char*)argv[1];
	account_query* acc_q = new account_query(dir);

	ERROR_CODE* err;
	//max account seq     max address seq
	cout << "max account seq: " << acc_q->account_size() << endl;
	cout << "max address seq: " << acc_q->address_size() << endl;

	//use account seq to get account
	account* acc = new account();
	acc = acc_q->get_account_obj(0, err);
	cout << "the account sequence :" << acc->get_account_seq() << endl;
	cout << "the size of address set :" << acc->get_size() << endl;



	ADDR_SEQ addr = 10;
	address_set* addr_set= new address_set(addr);
	cout<<"size of address set :"<<addr_set->get_size()<<endl;
	ADDR_SEQ* temp = new ADDR_SEQ[5];
	for (int i = 0; i < 5; i++)
		temp[i] = i;
	address_set* addr_set2 = new address_set(temp,5);
	cout << "size of address set :" << addr_set2->get_size()<<endl;
	addr = 5;
	addr_set2->push_back(addr);
	cout << "size of address set :" << addr_set2->get_size()<<endl;
    
	address_set addr_set3 = *addr_set + *addr_set2;
	cout << "size of address set :" << addr_set3.get_size()<<endl;

	
	




}