#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include <stdint.h>
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
#include <jsoncpp/json/json.h>
using namespace std;


void split(const string &str,vector<string> &res,const char pattern)
{
 istringstream is(str);
 string temp;
 while(getline(is,temp,pattern))
      res.push_back(temp);
 return ;

}

double string_to_double(string str)
{
 double res;
 stringstream ss;
 ss << str;
 ss >> res;

 return res;
}

int string_to_int(string str)
{
 int res;
 stringstream ss;
 ss << str;
 ss >> res;
 
 return res;

}

string double_to_string(double dou)
{
 string res;
 stringstream ss;
 ss << dou;
 ss >> res;
 
 return res;

}

string int_to_string(int i)
{
 string res;
 stringstream ss;
 ss << i;
 ss >> res;

 return res;
}

string int64_t_to_string(int64_t i)
{
 string res;
 stringstream ss;
 ss << i;
 ss >> res;

 return res;
}


int main(int argc, char *argv[])
{

	//命令行参数
	string input_account,json,output_path,file_num,acc_meta;
	int filenum,addnum;
	if(argc > 5)
    {
	input_account = argv[1];
	json = argv[2];
	output_path = argv[3];
	file_num = argv[4];
	acc_meta = argv[5];
	filenum = atoi(file_num.c_str());
    } 
    else
    {
        cout<<"error,the parameters are less than 5 "<<endl;
        return 0;
    }	


	//读入json文件tx_0.txt
	ifstream f;
	string line;
	f.open(acc_meta);
	while (getline(f, line))
	{
		int time, acc_num;
		stringstream input(line);
		input >> time >> acc_num >> addnum;
	}
	f.close();

           
	f.open(input_account);
	int* account = new int[addnum];
	for (int i = 0; i < addnum; i++)
		account[i] = -1;

	int lin = 0;
	while (getline(f, line))
	{
		lin++;
		printf("这是第%d次读取账户-----", lin);
		printf("\r\033[k");

		stringstream input(line);
		int addr, acc, total;
		input >> addr >> acc >> total;
		account[addr] = acc;              
	}
     cout<<"account done"<<endl;
	f.close();

	ofstream out_file;
	
	string last_txhash="";
    int l_json = 0;
	//用户编号，从0开始
	int user_number = 0;

	//处理了多少条记录
	lin = 0;
	int err = 0;
        int mult = 0;
	int abn = 0;
	int proc = 0;
	//发现了多少地址
	int add = 0;

	for (int i = 0; i < filenum; i++)
	{
		//读取二十个文件

		string number = int_to_string(i);
		string in_f = json + number + ".txt";
		string out_f = output_path + number + ".txt";
		f.open(in_f.data());
		out_file.open(out_f.data());

		while (getline(f, line))
		{
			//处理的记录总数
			lin++;
			printf("这是第%d次输出-----", lin);
		        printf("\r\033[k");

			unordered_map<int, long long> acc_out;
			unordered_map<int, long long>::iterator acc_it;
                        
                         
			vector<int> inputs;
			vector<int> outputs;

			vector<long long> in_val;
			vector<long long> out_val;
                        
			bool emflag = false;
			bool erflag = false;

			Json::Reader reader;
			Json::Value root;
			reader.parse(line, root);

			for (unsigned int i = 0; i < root["inputs"].size(); i++) {
				int address_num = root["inputs"][i]["address"].size();
				if (address_num > 1)
				{
					emflag = true;
					mult++;
					break;
				}

				//遍历一个交易输入
				for (unsigned int j = 0; j < address_num; j++) {
					int address_id = root["inputs"][i]["address"][j].asInt();

					//正常地址
					if (address_id >= 3)
					{
						inputs.push_back(address_id);
						//交易金额
						// string value = root["inputs"][i]["value"].asString();
						// long long v = stoll(value);
						double value = root["inputs"][i]["value"].asDouble();
						long long v = int64_t(value);
						in_val.push_back(v);
					}
					else if (address_id != 0)
					{
						erflag = true;
					}
					else
						inputs.push_back(address_id);
				}
			}

	
			//cout<<root["outputs"].size()<<endl;
			//交易输出

			for (unsigned int i = 0; i < root["outputs"].size(); i++) {
				int address_num = root["outputs"][i]["address"].size();

				//跳过多重签名交易
				if (address_num > 1)
				{
					emflag = true;
					mult++;
					break;
				}

				//遍历一个交易输出
				for (unsigned int j = 0; j < address_num; j++) {
					int address_id = root["outputs"][i]["address"][j].asInt();
					//正常地址
					if (address_id >= 3)
					{
						outputs.push_back(address_id);
						// string value = root["outputs"][i]["value"].asString();
						// long long v = stoll(value);
						double value = root["outputs"][i]["value"].asDouble();
						long long v = int64_t(value);
						out_val.push_back(v);
					}
					else if (address_id != 0)
					{
						erflag = true;
						
					}
					else
						;
				}
			}


			//输入输出地址数量
			int inputs_size = inputs.size();
			int outputs_size = outputs.size();
                     

		      if(emflag)
			{
			abn++;
			continue;
			}	
		       if(erflag)
		       {
			 abn++;
			 err++;
			 
		       }
                   

			proc++;
			//输入地址的账户只有一个
			long long acc_in_val = 0;
			int acc_in =-2;

		
			
			if (in_val.size() != 0)
			{

				int temp = 0;
				for (int i = 0; i < inputs.size(); i++)
				{
                                     
                    int input=inputs[i];
                                        
					long long inval=in_val[i];

					if (input >= 3)
					{
						temp = input;
						acc_in_val += inval;
                        acc_in=account[input];
					}
					 
				  }
			}                             
			else
				acc_in = 0;                    
			for (int i = 0; i < outputs_size; i++)
			{

                   
                int output = outputs[i];
				long long outval = out_val[i];

				if (output >= 3)
				{
					int temp = account[output];
					if (temp != -1)
					{
						int acc = account[output];
						acc_it = acc_out.find(acc);
						if (acc_it != acc_out.end())
						{
							acc_out[acc] += outval;
						}
						else
							acc_out[acc] = outval;
					}
				}
			}                       

			string txhash = root["txhash"].asString();
			string blocktime = root["blocktime"].asString();
			string fee = int64_t_to_string(int64_t(root["fee"].asDouble()));
            //same txhash
			if(txhash == last_txhash)
			{
             last_txhash = txhash;
			 l_json++;
			 continue;
			}
            last_txhash=txhash;
			//string change =  "{\"txhash\": \"" + txhash + "\",";
			////change +=  "\"blockhash\": \"" + blockhash + "\",";
			//change +=  "\"blocktime\": " + blocktime + ",";
			//change +=  "\"fee\": " + fee + ",";
			//change += "\"inputs\":[";
			out_file << "{\"txhash\": \"" + txhash + "\",";
			out_file << "\"blocktime\": " + blocktime + ",";
			out_file << "\"fee\": " + fee + ",";
			out_file << "\"inputs\":[";

			//输入账户
			if (acc_in == 0)
				out_file << "{\"account\": [0]}";
			else
			{
				/*change += "{\"value\": ";*/
				/*string val=double_to_string(acc_in_val);
				string s_acc=int_to_string(acc_in);
				change = change + val + ",\"account\": [";
				change = change + s_acc + "]}";*/
				out_file << "{\"value\": "<< acc_in_val << ",\"account\": [";
				out_file << acc_in << "]}";
			}

			out_file << "],\"outputs\":[";
			//输出账户
			int nSize = acc_out.size();
			acc_it = acc_out.begin();

			int n = 1;
			while (n <= nSize)
			{

				if (n == 1)
				out_file << "{\"value\": ";
				else
				out_file << ",{\"value\": ";
				/*string val=double_to_string(acc_it->second);
				string s_acc=int_to_string(acc_it->first);
				change = change + val + ",\"account\": [";
				change = change + s_acc + "]}";*/
				out_file << acc_it->second << ",\"account\": [";
				out_file << acc_it->first << "]}";
				n++;
				acc_it++;
			}
			
			
            

			out_file<<"]}"<<endl;
                        
                        
			
		}

		f.close();
	        out_file.close();	
	}
        
	

	cout << "input file: " << input_account << endl;
	cout << "json file: " << json << "0-"<< filenum-1 << ".txt"<<endl;
	cout << "output file: " << output_path <<"0-"<<filenum<<".txt" << endl;
	cout << "the number of records processed: "<< lin<< endl;
	cout << "the number of normal record: "<< proc <<endl;
	cout << "the number of abnormal records: "<< abn <<endl;
	cout << "the number of records have exception: "<< err << endl;
	cout << "the number of records have multiaddress: "<< mult <<endl;
	cout << "the number of same txhash: "<< l_json << endl;
	cout << "The run time is:" << (double)clock() /CLOCKS_PER_SEC<< "s" << endl;


	return 0;
}
