#include <string.h>
#include "../include/account.h"
#include "../include/account_query.h"
#include "../include/address_set.h"

account_query::account_query(char *account_dir){
    strcpy(addr2account_fname,account_dir);
    strcat(addr2account_fname,"addr2account.dat");

    strcpy(addr_list_fname,account_dir);
    strcat(addr_list_fname,"addr_list.dat");

    strcpy(account2addr_fname,account_dir);
    strcat(account2addr_fname,"account2addr.dat");

    strcpy(account_info_fname,account_dir);
    strcat(account_info_fname,"account_info.dat");

    //?
    //Read the max_account_seq and max_addr_seq from meta file
	//max_acc_seq
	ifstream database;
	database.open(account2addr_fname, ios::binary);
	database.seekg(0, ios::end);
	unsigned int database_length = database.tellg();
	max_account_seq = database_length / sizeof(ACCOUNT_SEQ) - 2;
	database.close();

	//max_addr_seq
	database.open(addr2account_fname, ios::binary);
	database.seekg(0, ios::end);
	database_length = database.tellg();
	max_addr_seq = database_length / sizeof(ADDR_SEQ) - 1;
	database.close();

}
ACCOUNT_SEQ account_query::get_account_seq(ADDR_SEQ seq, ERROR_CODE *err)
{
	//seq can not <0
	if (seq >max_addr_seq){
		*err = INVALID_ADDR_SEQ;
		return NULL_SEQ;
	}

	ACCOUNT_SEQ acc_seq;

	//open addr2account to find account sequence
	ifstream file(addr2account_fname, ios::binary);

	//can not open file
	if (!file.is_open()){
			*err = CANNOT_OPEN_FILE;
			return NULL_SEQ;
	}

	//find account sequence
	file.seekg(seq * sizeof(ACCOUNT_SEQ), ios::beg);
	file.read((char*)&acc_seq, sizeof(ACCOUNT_SEQ));
	file.close();

	*err = NO_ERROR;
	return acc_seq;
}
account* account_query::get_account_obj(ACCOUNT_SEQ seq,ERROR_CODE *err){
	
	//Load the addr seqs in the account 
	//from ACCOUNT2ADDR and ADDR_LIST files into the aset

	//get account begin address index and end address index

    if (seq >max_addr_seq){
		*err = INVALID_ADDR_SEQ;
		return NULL;
	}
	int begin = 0;
	int end = 0;
	ifstream file(account2addr_fname, ios::binary);
	file.seekg(seq * sizeof(int), ios::beg);
	file.read((char*)&begin, sizeof(int));
    //?
    //if seq==max_addr_seq
	//file.seekg((seq+1) * sizeof(int), ios::beg);
	file.read((char*)&end, sizeof(int));
	file.close();
	//cout<<begin<<" "<<end<<endl;
	//from addr_list file into aset
	account *cur_account=new account();
    cur_account->set_account_seq(seq);
	ifstream file2(addr_list_fname, ios::binary);
    file2.seekg(begin*sizeof(ADDR_SEQ), ios::beg);
	for (int i = begin; i < end; i++)
	{
		ADDR_SEQ addr_id ;
		file2.read((char*)&addr_id, sizeof(ADDR_SEQ));
//		cout<<"Read from file:"<<addr_id<<endl;
		cur_account->push_back(addr_id);
	}
	file2.close();

    return cur_account;
}
ERROR_CODE account_query::get_account_info(ACCOUNT_SEQ seq,struct account_info *info){
	//Read the account information from the ACCOUNT_INFO file to p
	ifstream file(account_info_fname, ios::binary);
	if (!file) {
		cout << "can not open file" << endl;
		return CANNOT_OPEN_FILE;
	}
	else
	{
		//size of file
		file.seekg(0, ios::end);
		int database_length = file.tellg();
		int size = database_length / sizeof(account_info);

		//out of memory
		if (seq >= size)
			return CACHE_OUT_MEMORY;

		//get account info
		file.seekg(seq * sizeof(account_info), ios::beg);
		file.read((char*)info, sizeof(account_info));
		file.close();
	}
	return NO_ERROR;
}

ADDR_SEQ account_query::address_size()
{
	return max_addr_seq + 1;
}

ACCOUNT_SEQ account_query::account_size()
{
	return max_account_seq + 1;
}
