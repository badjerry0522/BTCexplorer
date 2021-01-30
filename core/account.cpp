#include "../include/account.h"


account::account(ACCOUNT_SEQ seq){
	
	//Load the addr seqs in the account 
	//from ACCOUNT2ADDR and ADDR_LIST files into the aset

	//get account begin address index and end address index
	int begin = 0;
	int end = 0;
	ifstream file("account2addr.dat", ios::binary);
	file.seekg(seq * sizeof(int), ios::beg);
	file.read((char*)&begin, sizeof(int));
	file.seekg((seq+1) * sizeof(int), ios::beg);
	file.read((char*)&end, sizeof(int));
	file.close();

	//from addr_list file into aset
	set<ADDR_SEQ> addr_list;
	ifstream file2("addr_list.dat", ios::binary);
	for (int i = begin; i < end; i++)
	{
		int addr_id ;
		file2.seekg(i * sizeof(ADDR_SEQ), ios::beg);
		file2.read((char*)&addr_id, sizeof(ADDR_SEQ));
		addr_list.insert(addr_id);
	}
	file2.close();

	//set address_set aset
	address_set::set_aset(addr_list);
   
}

ACCOUNT_SEQ account::get_account_seq(){
	return seq;
}

ERROR_CODE account::get_account_info(struct account_info *info){
	//Read the account information from the ACCOUNT_INFO file to p
	ifstream file("account_info.dat", ios::binary);
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
		file.read((char*)&info, sizeof(account_info));
		file.close();
	}
	return NO_ERROR;
}


