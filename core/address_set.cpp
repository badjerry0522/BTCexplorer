#include <set>
#include "../include/core_type.h"
#include "../include/address_set.h"

address_set::address_set(){
	size=0;
}
//One address 
address_set::address_set(ADDR_SEQ addr)
{
	aset.insert(addr);
}

//Multi addresses 
address_set::address_set(ADDR_SEQ *p, int num)
{
	for(int i=0;i<num;i++){
		aset.insert(p[i]);
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
		int addr;
		file.seekg(0, ios::beg);
		while (file.read((char *)&addr, sizeof(ADDR_SEQ)))
		{
			//until the end of the file
			aset.insert(addr);
		}
		file.close();

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

address_set address_set::operator+(const address_set &addr_set1)
{

	//add two address_set 
	address_set addr_set2;
	for (set<ADDR_SEQ>::iterator it = addr_set1.aset.begin(); it != addr_set1.aset.end(); it++)
	{
		addr_set2.aset.insert(*it);
	}
	for (set<ADDR_SEQ>::iterator it = this->aset.begin(); it != this->aset.end(); it++)
	{
		addr_set2.aset.insert(*it);
	}
	return addr_set2;
}

address_set address_set::operator-(const address_set &addr_set1)
{
	address_set addr_set2;
	set<ADDR_SEQ> s = addr_set1.aset;

	//substract two address_set
	for (set<ADDR_SEQ>::iterator it = this->aset.begin(); it != this->aset.end(); it++)
	{
		if (!s.count(*it))
			addr_set2.aset.insert(*it);
	}
	return addr_set2;
}

address_set::~address_set(){
	aset.clear();
}

ADDR_SEQ address_set::begin(){
	return NULL_SEQ;
}
ADDR_SEQ address_set::next(){
	return NULL_SEQ;
}