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
	string addrtxt, result, addr_num;
	int addrnum;
	if (argc > 3) {
			 addrtxt = argv[1];  // 账户地址的聚类文件
			 result = argv[2];   // 输出结果文件
			 addr_num = argv[3];  // 账户数
			 addrnum = atoi(addr_num.c_str());
	}
	else{
			cout << "error,the parameters are less than 2 " << endl;
			return 0;
	}

	//标签地址文件


	int* addr = new int[addrnum];

	ifstream infile;
	infile.open(addrtxt.data());
	assert(infile.is_open());

	vector<int> addressids;
	fstream memoryfile(result, ios::out | ios::binary);
	

	//读取地址id
	int k = 0;
	string s;
	while (getline(infile, s))
	{
		printf("这是第%d次读取账户-----", k);
		printf("\r\033[k");
	
		istringstream input(s);
		int addrid, accid, addnum;
		input >> addrid >> accid >> addnum;
	    
		addr[addrid] = accid;
		

		//memoryfile.write((char*)&temp, sizeof(temp));
		k++;
	}
	infile.close();

	for(int i=0;i<addrnum;i++)
	{
	  memoryfile.write((char*)&addr[i], sizeof(int));
	 }
	memoryfile.close();


	//测试

	  int re;
	  fstream iofile1(result, ios::in | ios::binary);
	  iofile1.seekg(50 * sizeof(re), ios::beg);
	  iofile1.read((char*)&re, sizeof(re));
 
	
	  cout << re  << endl;


}
