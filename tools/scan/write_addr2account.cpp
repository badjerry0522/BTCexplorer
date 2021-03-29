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

//���׵�ַ����Ĳ���




int main(int argc, char *argv[])
{
	string addrtxt, result, meta;
	int addrnum;
	if (argc > 3) {
			 addrtxt = argv[1];  // �˻���ַ�ľ����ļ�
			 result = argv[2];   // �������ļ�
			 meta= argv[3];  // meta
			 
	}
	else{
			cout << "error,the parameters are less than 3 " << endl;
			return 0;
	}
	ifstream f;
	string line;
	f.open(meta);
	while (getline(f, line))
	{
		int time, acc_num;
		stringstream input(line);
		input >> time >> acc_num >> addrnum;
	}
	f.close();



	int* addr = new int[addrnum];

	ifstream infile;
	infile.open(addrtxt.data());
	assert(infile.is_open());

	vector<int> addressids;
	fstream memoryfile(result, ios::out | ios::binary);
	

	//��ȡ��ַid
	int k = 0;
	string s;
	while (getline(infile, s))
	{
		printf("���ǵ�%d�ζ�ȡ�˻�-----", k);
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


	//����

	  int re;
	  fstream iofile1(result, ios::in | ios::binary);
	  iofile1.seekg(50 * sizeof(re), ios::beg);
	  iofile1.read((char*)&re, sizeof(re));
 
	
	  cout << re  << endl;


}
