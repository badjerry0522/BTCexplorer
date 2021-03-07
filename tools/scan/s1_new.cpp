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

struct address{
	int address;
	int root;
	int root_index;
};

#define MAX_INPUT_SIZE 40960
#define MAX_OUTPUT_SIZE 40960
#define INVALID_ADDR 0x7FFFFFFF

int *root;  //2G 
int *account;//2G

struct address input_addrlist[MAX_INPUT_SIZE];
struct address output_addrlist[MAX_OUTPUT_SIZE];

void updateroots(int* root,address* addrlist,int address_num,int rootindex)
{
	for (int i = 0; i < address_num; i++)
	{
		if (root[addrlist[i].address] == INVALID_ADDR)
			root[addrlist[i].address] = rootindex;
		else
		{
			root[addrlist[i].address] = rootindex;
			root[addrlist[i].root_index] = rootindex;
		}
	}
}

int chooseroot(int* root,address* addrlist,int address_num)
{

	int min_index = addrlist[0].address;
	for (int i = 0; i < address_num; i++)
	{
		if (root[addrlist[i].address] != INVALID_ADDR)
		{
			int index = addrlist[i].address;
			while (index != root[index] )
			{
				index = root[index];
			}
			addrlist[i].root = root[index];
			addrlist[i].root_index = index;
		}
		else
		{

			addrlist[i].root = addrlist[i].address;
			addrlist[i].root_index = addrlist[i].address;

		}
	
	}
	return min_index;
}

int is_new(int* root,address* addrlist,int address_num)
{
	for (int i = 0; i < address_num; i++)
	{
		if (root[addrlist[i].address] != INVALID_ADDR)
			     return 0;		
	}
	return 1;

}

int main(int argc, char *argv[])
{
    
     string input_path,output_path,file_num,add_num;
	int filenum,addnum;
	//命令行参数
	if(argc > 4)
	{
        input_path = argv[1];
	    output_path = argv[2];
	    file_num = argv[3];
		add_num = argv[4];
        filenum = atoi(file_num.c_str());
		addnum = atoi(add_num.c_str());
	}
	else
	{
        cout<<"error,the parameters are less than 4 "<<endl;
	return 0;
	}

	//初始化数组
	root = new int[addnum];

	for (int i = 0; i < addnum; i++)
		root[i] = INVALID_ADDR;

	//读入json文件tx_0.txt
	ifstream f;
	string line;

	ofstream out_file, invalid_addr;
	out_file.open(output_path);
	/*invalid_addr.open("invalid_addr.txt");*/
	//多重签名交易的数量
    int mult=0;
	//记录的序列
	int seq = 0;
	//含有错误地址的交易的数量
	int err = 0;
	//正常的交易
    int proc = 0;
	//不正常的交易
	int abn = 0;
	//发现了多少地址
	int add = 0;
	int invalid_input = 0;

     for(int i=0;i<filenum;i++)
    {
	  //读取若干个文件

	  string number;
	  stringstream ss;
      ss << i;
      ss >> number;	  
      string in_f=input_path+number+".txt";	       
	  f.open(in_f.data());     

	while (getline(f, line))
	{
		//记录总数
		
        printf("这是第%d次交易-----",seq);
        printf("\r\033[k");

		int in_address_num = 0;
		int out_address_num = 0;

		bool emflag = false;
		bool erflag = false;
		bool erflag_in = false;

		Json::Reader reader;
		Json::Value root_json;
		reader.parse(line, root_json);


		//输入输出地址集
		
	
		for (unsigned int i = 0; i < root_json["inputs"].size(); i++) {
			int address_num = root_json["inputs"][i]["address"].size();
			if (address_num > 1)
			{
				emflag = true;
				break;
			}

			//遍历一个交易输入
			
			for (unsigned int j = 0; j < address_num; j++) {
				int address_id = root_json["inputs"][i]["address"][j].asInt();

				//正常地址
				if (address_id >= 3)
				{
					//Tommy Add
					if(in_address_num>MAX_INPUT_SIZE){
						printf("Error input_num>%d\n",MAX_INPUT_SIZE);
						exit(0);
					}
					input_addrlist[in_address_num].address=address_id;
					in_address_num++;

				}
				else if (address_id != 0)
				{
					erflag = true;
				}
				else
				{
					input_addrlist[in_address_num].address = address_id;
					in_address_num++;

				}
			}
		}
		//cout<<root["outputs"].size()<<endl;
		//交易输出
		
		for (unsigned int i = 0; i < root_json["outputs"].size(); i++) {
			int address_num = root_json["outputs"][i]["address"].size();

			//跳过多重签名交易
			if (address_num > 1)
			{
				emflag = true;
				break;
			}

			//遍历一个交易输出
			for (unsigned int j = 0; j < address_num; j++) {
				int address_id = root_json["outputs"][i]["address"][j].asInt();
				//正常地址
				if (address_id >= 3)
				{
					if (out_address_num > MAX_INPUT_SIZE) {
						printf("Error output_num>%d\n", MAX_INPUT_SIZE);
						exit(0);
					}
					output_addrlist[out_address_num].address = address_id;
					out_address_num++;

				}
				//Tommy Add 输出加入output_addrlist
				else if (address_id != 0)
				{
					erflag = true;
				
				}
				else
					;
			}
		}
	
		 
         if(emflag)
		{
		  abn++;
		  mult++;
		  seq++;
		  continue;
		}

		 if (erflag)
		 {
			 abn++;
			 err++;
			 invalid_input++;
			/* if (in_address_num > 0) {
				 invalid_addr << line << endl;
				 invalid_input++;
			 }*/
		 }
		 
                
		
		 int rootindex = -1;

	

		//处理输入地址
		if (input_addrlist[0].address >= 3)
		{
			//正常交易 根据条件判断inputs和outputs的关系                     
			//处理inputs   H1
			//Tommy Addr
			
			
				//有不是第一次出现的地址
				rootindex = chooseroot(root,input_addrlist, in_address_num);
				updateroots(root, input_addrlist, in_address_num, rootindex);
			
			//处理outputs  H2 H3
			
                        
		}

		

		//处理输出地址
		for (int i = 0; i < out_address_num; i++)
		{
			if (root[output_addrlist[i].address] == INVALID_ADDR)
				root[output_addrlist[i].address] = output_addrlist[i].address;
		}



		proc++;
		seq++;
	}

	f.close();

     }
	 invalid_addr.close();

	 unordered_map<int, set<int>> accMap;

	 for (int i=0;i<addnum;i++)
	 {
		 printf("读取第%d个地址-----",add);
		 printf("\r\033[k");
		
		 int t = root[i];

		 if (root[i] != INVALID_ADDR)
		 {

			 while (t != root[t])
			 {
				 t = root[t];
			 }

			 /*accMap[t].insert(i);*/
			 out_file << i << " " << t << endl;
			 add++;
		 }
		 else
			 out_file << i << " " << i<< endl ;
	 }
	 cout<< endl << "the number of address: " << add << endl;



	//int t_user_size = 0;


	//cout << "the number of account: " << accMap.size() << endl;

	//for (auto i : accMap)
	//{
	//	int len = i.second.size();
	//	printf("写入第%d个账户-----", t_user_size);
	//	printf("\r\033[k");
	//	for (auto j : i.second)
	//	{
	//		out_file << j << " " << i.first << " " << len << endl;
	//	}
	//	t_user_size++;
	//}


	out_file.close();
        

	
	cout << "input file: " << input_path<<"0-"<<filenum-1<<".txt"<< endl;
	cout << "output file: " << output_path << endl;
	cout << "the number of records processed: " << seq << endl;
	cout << "the number of normal records : " << proc <<endl;
	cout << "the number of abnormal records: "<< abn <<endl;
	cout << "the number of records have exception: "<<err<<endl;
	cout << "the number of records have multiaddress: "<<mult<<endl;
	cout << "the number of records have invalid and valid iaddress: " << invalid_input << endl;
	cout << "The run time is:" << (double)clock() /CLOCKS_PER_SEC<< "s" << endl;

	
	return 0;
}
