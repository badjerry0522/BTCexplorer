#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cassert>
#include <sstream>
#include <time.h>
#include <algorithm> 
#include <unordered_map>


using namespace std;

//交易地址方面的参数



int main(int argc, char *argv[])
{
	string addrtxt, addrlist,account2addr, acc_num;
	int accnum;
	if (argc > 3) {
			 addrtxt = argv[1];  // 账户地址的聚类文件
			 account2addr = argv[2];
			 addrlist = argv[3];
	}
	else{
			cout << "error,the parameters are less than 2 " << endl;
			return 0;
	}

	//标签地址文件


	ifstream infile;
	infile.open(addrtxt.data());
	assert(infile.is_open()); 

	ofstream memoryfile2(addrlist.data(), ios::binary);
	ofstream memoryfile3(account2addr.data(),ios::binary);

	//读取地址id
	int k = 0;
	int last = 0;
	string s;
	memoryfile3.write((char*)&last, sizeof(int));
	while (getline(infile, s))
	{
		printf("这是第%d次读取账户-----", k);
		printf("\r\033[k");
	
		istringstream input(s);
		int addrid, accid, addnum;
		input >> addrid >> accid >> addnum;
	    

		memoryfile2.write((char*)&addrid, sizeof(int));
		if (last != accid) {
			memoryfile3.write((char*)&k, sizeof(int));
			last = accid;
		}
		k++;
	}
	memoryfile3.write((char*)&k, sizeof(int));

	infile.close();
	memoryfile2.close();
	memoryfile3.close();


	////测试

	//  int begin = 0;
	//  int end = 0;
	//  ifstream file("test2.dat", ios::binary);
	//  file.seekg(4 * sizeof(int), ios::beg);
	//  file.read((char*)&begin, sizeof(int));
	//  file.seekg(5 * sizeof(int), ios::beg);
	//  file.read((char*)&end, sizeof(int));
	//  file.close();
	//  file.open("test.data", ios::binary);
	//  for (int i = begin; i < end; i++)
	//  {
	//	  int addr_id;
	//	  
	//	  file.seekg(i * sizeof(int), ios::beg);
	//	  file.read((char*)&addr_id, sizeof(int));
	//	  cout << addr_id << endl;
	//  }
	//  file.close();

	 

}
