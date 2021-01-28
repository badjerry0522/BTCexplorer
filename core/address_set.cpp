#include <set>
#include "../include/core_type.h"
#include "../include/address_set.h"


//One address 
address_set::address_set(ADDR_SEQ addr)
{
	aset.insert(addr);


	//?
	address_set_info addr_info;

	ifstream file("address_info.dat", ios::binary);
	if (!file) {
		cout << "can not open file" << endl;
	}
	else
	{
		file.seekg(addr * sizeof(address_set_info), ios::beg);
		file.read((char*)&addr_info, sizeof(address_set));
		file.close();

		//bug!
		this->addr_set_info = &addr_info;
		ADDR_SEQ temp = addr;
		this->addresses = &temp;
		this->size = 1;
	}
}

//Multi addresses 
address_set::address_set(ADDR_SEQ *p, int num)
{
	for(int i=0;i<num;i++){
		aset.insert(p[i]);
	}

	//?
	address_set_info* addr_info = new address_set_info[num];
	this->addresses = p;
	

	ifstream file("address_info.dat", ios::binary);
	if (!file) {
		cout << "can not open file" << endl;
	}
	else
	{
		for (int i = 0; i < num; i++)
		{
			int seq = *(p + i);
			file.seekg(seq * sizeof(address_set_info), ios::beg);
			file.read((char*)&addr_info[i], sizeof(address_set_info));
			p++;
		}
		file.close();

		this->size = num;
		this->addr_set_info = &addr_info[0];
	}
}
//Load BTC addresses file
address_set::address_set(char *filename)
{

	//open address sequence file
	ifstream file(filename, ios::binary);
	if (!file) {
		cout << "can not open file" << endl;
	}
	else
	{
		//size of file;
		file.seekg(0, ios::end);
		int database_length = file.tellg();
		int size = database_length / sizeof(ADDR_SEQ);
		file.seekg(0, ios::beg);

		ADDR_SEQ* addrs = new ADDR_SEQ[size];
		int index = 0;
		while (file.read((char *)&addrs[index], sizeof(addrs[index])))
		{
			//until the end of the file
			index++;
		}
		file.close();



		address_set_info* addr_info = new address_set_info[size];

		//open address infomation file
		ifstream file_info("address_info.dat", ios::binary);
		if (!file_info) {
			cout << "can not open file" << endl;
		}
		else
		{
			index = 0;
			while (file_info.read((char *)&addr_info[index], sizeof(addr_info[index])))
			{
				//until the end of the file
				index++;
			}
			file_info.close();


			this->size = size;
			this->addr_set_info = &addr_info[0];
			this->addresses = &addrs[0];
		}
	}
}
int address_set::push_back(ADDR_SEQ addr){
	if(isIn(addr)) return 1;
	aset.insert(addr);
	return 0;
}
int address_set::isIn(ADDR_SEQ addr){
	set<ADDR_SEQ>::iterator iter;
	iter=aset.find(addr);
	if(iter==aset.end())
		return 0;
	return 1;
}
void address_set::set_label(LABEL l){
	label=l;
}
LABEL address_set::get_label(){
	return label;
}

void address_set::set_name(string name1){
	name=name1;
}
string address_set::get_name(){
	return name;
}

int address_set::get_size()
{
	return aset.size();
}