#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <cassert>
#include <sstream>
#include <time.h>
#include <jsoncpp/json/json.h>
using namespace std;





int main(int argc, char *argv[])
{
    
     string input_path,output_path,file_num,record_num;
	int filenum,recordnum;
	//命令行参数
	if(argc > 2)
	{
        input_path = argv[1];
	    output_path = argv[2];
	}
	else
	{
        cout<<"error,the parameters are less than 2 "<<endl;
	return 0;
	}


	//读入json文件tx_0.txt
	ifstream f;
	string line;
	f.open(input_path);
	

	//用户编号，从0开始
	int user_number = 0;

	//处理了多少条记录
	int lin = 0;
	map<int, set<int>> accMap;

	while (getline(f, line))
	{
		lin++;
		printf("这是第%d次读取地址-----", lin);
		printf("\r\033[k");

		stringstream input(line);
		int addr, acc;
		
		
		input >> addr >> acc;
		
		accMap[acc].insert(addr);
	}

	 cout<<endl << "the number of address: " << lin << endl;

	
	 ofstream out_file;
	 out_file.open(output_path,ios::app);
     


	int t_user_size = 0;
	

	

	for (auto i : accMap)
	{
		int len = i.second.size();
		printf("写入第%d个账户-----", t_user_size);
		printf("\r\033[k");
		if (i.first == -1)
		{
			for (auto j : i.second)
			{
				out_file << j << " " << t_user_size << " " << 1 << endl;
				t_user_size++;
			}
		}
		else
		{
			for (auto j : i.second)
			{
				out_file << j << " " << t_user_size << " " << len << endl;
			}
		}
		t_user_size++;
	}


	out_file.close();
	cout << "the number of account: " << t_user_size << endl;
	
	cout << "input file: " << input_path<< endl;
	cout << "output file: " << output_path << endl;
		
	cout << "The run time is:" << (double)clock() /CLOCKS_PER_SEC<< "s" << endl;

	
	return 0;
}
