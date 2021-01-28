#include "address_query.h"




ACCOUNT_SEQ address_query::get_account_seq(ADDR_SEQ seq, ERROR_CODE *err)
{
	//seq can not <0
	if (seq < 0)
	{
		*err = INVALID_ADDR_SEQ;
		return 0;
	}

	ACCOUNT_SEQ acc_seq;

	//open addr2account to find account sequence
	ifstream file("addr2account.dat", ios::binary);

	//can not open file
	if (!file.is_open())
	{
			std::cout << "Error: Fail to open database file." << std::endl;
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