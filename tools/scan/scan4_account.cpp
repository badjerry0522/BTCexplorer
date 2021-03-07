#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>
#include <unordered_set>
#include <map>
using namespace std;

const double EPS = 1e-20;

void split(const string& s,vector<string>& sv,const char flag=' ')
{
   sv.clear();
   istringstream iss(s);
   string temp;

   while(getline(iss,temp,flag))
   {
      sv.push_back(temp);
   }
   return ;
}

struct addr_info
{

	int firsttime;   //最早出现时间
	int lasttime;    //最晚出现时间
	int K;           //参与交易总数
	int Kin;         //存入交易总数
	int Kout;        //取出交易总数
	long long V;        //当前账户金额（单位：聪）
	long long Vmax;     //历史最大金额（单位：聪）
	long long Vin;      //交易输入金额总数（单位：聪）
	long long Vout;     //交易输出金额总数（单位：聪）
	bool Multisig;   //是否为多地址类型
	int sumout;      //取出交易的账户地址总数
	int sumin;       //存入交易的账户地址总数

	addr_info() {
		firsttime = -10;
		lasttime = -10;
		K = 0;
		Kin = 0;
		Kout = 0;
		V = 0;
		Vmax = 0;
		Vin = 0;
		Vout = 0;
		Multisig = 0;  //false
		sumout = 0;
		sumin = 0;
	}
};

struct addr_info2 {
	int PROD;      //活跃时长（单位：秒）
	float PRKIKO;  //输入交易和输出交易之比
	float TS;      //交易频度
	bool VZERO;    //当前金额是否为 0
	float VL;      //交易金额总数和最大金额之比
	int T10;       //在 [T1, TK] 时间范围内地址金额为最大金额 10% 以下时间（单位：秒）
	int T1050;     //在 [T1, TK] 时间范围内金额为最大金额 10%～50% 时间（单位：秒）
	int T50;       //在 [T1, TK] 时间范围内地址金额为最大金额 50% 以上时间（单位：秒）
	float PRT10;   //地址金额为最大金额 10% 以下时间占比
	float PRT1050; //地址金额为最大金额 10%～50% 时间占比
	float PRT50;   //地址金额为最大金额 50% 以上时间占比
	float ARANUM;  //参与交易的平均地址数
	//int ANUM;    //地址所属账户中包含的地址数

	addr_info2() {
		PROD = 0;      //活跃时长（单位：秒）
		PRKIKO = 0.0;  //输入交易和输出交易之比
		TS = 0.0;      //交易频度
		VZERO = 0;     //(false)当前金额是否为 0
		VL = 0.0;      //交易金额总数和最大金额之比
		T10 = 0;       //在 [T1, TK] 时间范围内地址金额为最大金额 10% 以下时间（单位：秒）
		T1050 = 0;     //在 [T1, TK] 时间范围内金额为最大金额 10%～50% 时间（单位：秒）
		T50 = 0;       //在 [T1, TK] 时间范围内地址金额为最大金额 50% 以上时间（单位：秒）
		PRT10 = 0.0;   //地址金额为最大金额 10% 以下时间占比
		PRT1050 = 0.0; //地址金额为最大金额 10%～50% 时间占比
		PRT50 = 0.0;   //地址金额为最大金额 50% 以上时间占比
		ARANUM = 0.0;  //参与交易的平均地址数
		//ANUM  = 0;   //地址所属账户中包含的地址数
	}
};

struct addr_info3 {
	int WANUM = 0; //账户地址数
	int WMAXAPROD = 0;  //账户中地址的最长活跃时长
	float WAVGAPROD = 0.0; //账户中地址的平均活跃时长
	float WPMAXAPROD = 0.0; //账户中地址的最长活跃时长与账户活跃时长之比
	float WPAVGAPROD = 0.0; //账户中地址的平均活跃时长与账户活跃时长之比 
	long long WMAXAV = 0.0; //账户中地址的最大比特币数量
	float WPMAXAV = 0.0; //账户中地址的最大比特币数量占账户的最大比特币数量
};



int string_to_int(string str)
{
  int res;
  stringstream ss;
  ss << str;
  ss >> res;
  return res;
}

double string_to_double(string str)
{
  double res;
  stringstream ss;
  ss << str;
  ss >> res;
  return res;
  
}



int main(int argc, char *argv[])
{

 string id_account_num,data2_account,data3_account,data2_address,data3_address,accnum,addnum;
 int acc_num,add_num;
 if(argc > 5)
 {
     id_account_num = argv[1];
     data2_account = argv[2];
     data3_account = argv[3];
     data2_address = argv[4];
     data3_address = argv[5];
	 accnum = argv[6];
	 addnum = argv[7];
	 acc_num = atoi(accnum.c_str());
	 add_num = atoi(addnum.c_str());
 }
 else
 {
  cout<<"error,the parameters are less than 5 "<<endl;
  return 0;
 }

 

 string line;

 int addr,acc,total;



  
 ofstream csv_file;

 
 long long* account = new long long[acc_num];  //账户最大比特币数量
 int* account2 = new int[acc_num]; //账户活跃时长
 long long* address = new long long[add_num]; //地址最大比特币数量
 int* address2 = new int[add_num]; //地址活跃时长

 int index = 0;
 int num = 0;
 cout<<endl<<"acc2"<<endl;
 fstream file2, file3, file4, file5;
 fstream file(data2_account, ios::in | ios::binary);
 addr_info temp;
 addr_info2 temp2;
 while (file.read((char *)&temp, sizeof(addr_info)))
 {
	 account[index] = temp.Vmax;
	 index++;
 }
 file.close();
 cout << num << endl;
 index=0;
 num = 0;
 file2.open(data3_account, ios::in | ios::binary);
 cout<<endl<<"acc3"<<endl;
 while(file2.read((char *)&temp2, sizeof(addr_info2)))
 {
	 account2[index] = temp2.PROD;
     index++;
 }
 file2.close();
 cout << num << endl;
 cout<<endl<<"account done"<<endl;

 num = 0;
 index = 0;
 cout<<endl<<endl<<"addr2"<<endl;
 file3.open(data2_address, ios::in | ios::binary);
 while(file3.read((char *)&temp, sizeof(addr_info)))
 {
  address[index] = temp.Vmax;
  index++;
 }
 file3.close();
 cout << num << endl;
 index = 0;
 num = 0;
 file4.open(data3_address, ios::in | ios::binary);
 cout<<endl<<"addr3"<<endl;
 while(file4.read((char *)&temp2, sizeof(addr_info2)))
 {
  address2[index] = temp2.PROD;
  index++;
 }
 file4.close();
 cout << num << endl;
 cout<<endl<<"address done"<<endl;

 file5.open(id_account_num);
 int last = -1;
 int WANUM = 0; //账户地址数
 int WMAXAPROD = 0;  //账户中地址的最长活跃时长
 float WAVGAPROD = 0.0; //账户中地址的平均活跃时长
 float WPMAXAPROD = 0.0; //账户中地址的最长活跃时长与账户活跃时长之比
 float WPAVGAPROD = 0.0; //账户中地址的平均活跃时长与账户活跃时长之比 
 long long WMAXAV = 0; //账户中地址的最大比特币数量
 float WPMAXAV = 0.0; //账户中地址的最大比特币数量占账户的最大比特币数量


  index = 0;
 
 csv_file.open("data4_account.csv");
 ofstream record("record.txt");
 fstream memoryfile("memory4_account.dat", ios::out | ios::binary);
 //int addr,acc,total;
 //vector<string> temp,temp2,temp3,temp4;

 while(getline(file5,line))
 {

    printf("这是第%d次输出-----",index);
    printf("\r\033[k");


    stringstream input(line);
    input>>addr>>acc>>total;

    if(last==-1)
    last = 0;	   


    if(last==acc)
    {
     WANUM = total;
     int time = address2[addr];

     if(time>=WMAXAPROD)
	   WMAXAPROD=time;
	 WAVGAPROD = WAVGAPROD + ((float)time / (float)WANUM);
	 

     long long val=address[addr];

     if(val>=WMAXAV)
     	WMAXAV=val;

    }
    else 
    {
     
	int WPROD = account2[last]; //账户活跃时长
	if (WPROD > 0)
		WPMAXAPROD = (float)WMAXAPROD / (float)WPROD;
	else if (WPROD == 0)
		WPMAXAPROD = 0.0;
	else
		record << "WPROD:" << addr << " " << WPROD << endl;
	if (WPROD > 0)
		WPAVGAPROD = (float)WAVGAPROD / (float)WPROD;
	else if (WPROD == 0)
		WPAVGAPROD = 0.0;
	else
		;
	long long WVMAX = account[last];
	if(WVMAX==0)
        WPMAXAV = 0.0;
    else
	    WPMAXAV = (float)WMAXAV / (float)WVMAX;	
         

	    addr_info3 account3;
		account3.WANUM = WANUM;
		account3.WMAXAPROD = WMAXAPROD;
		account3.WAVGAPROD = WAVGAPROD;
		account3.WPMAXAPROD = WPMAXAPROD;
		account3.WPAVGAPROD = WPAVGAPROD;
		account3.WMAXAV = WMAXAV;
		account3.WPMAXAV = WPMAXAV;
		memoryfile.write((char*)&account3, sizeof(account3));
        //写入文件
	    csv_file<<last<<",";
	    csv_file<<WANUM<<",";
        csv_file<<WMAXAPROD<<",";
        csv_file<<WAVGAPROD<<",";
        csv_file<<WPMAXAPROD<<",";
        csv_file<<WPAVGAPROD<<",";
        csv_file<<WMAXAV<<",";
        csv_file<<WPMAXAV<<endl;	
        



	//重设
    WANUM = 0;
    WMAXAPROD = 0;
	WAVGAPROD = 0.0;
	WPMAXAPROD = 0.0;
	WPAVGAPROD = 0.0;
	WMAXAV = 0;
	WPMAXAV = 0.0;
     



	WANUM = total;
	int time = address2[addr];

	if (time >= WMAXAPROD)
		WMAXAPROD = time;


	WAVGAPROD = WAVGAPROD + ((float)time / (float)WANUM);
	long long val = address[addr];

	if (val >= WMAXAV)
		WMAXAV = val;





	last = acc;
 
    }
    index++;   
 }
 


 int WPROD = account2[last]; //账户活跃时长
 if (WPROD > 0)
	 WPMAXAPROD = (float)WMAXAPROD / (float)WPROD;
 else if (WPROD == 0)
	 WPMAXAPROD = 0.0;
 else
	 record << "WPROD:" << addr << " " << WPROD << endl;
 if (WPROD > 0)
	 WPAVGAPROD = (float)WAVGAPROD / (float)WPROD;
 else if (WPROD == 0)
	 WPAVGAPROD = 0.0;
 else
	 ;
 long long WVMAX = account[last];
 if (WVMAX == 0)
	 WPMAXAV = 0.0;
 else
	 WPMAXAV = (float)WMAXAV / (float)WVMAX;



 addr_info3 account3;
 account3.WANUM = WANUM;
 account3.WMAXAPROD = WMAXAPROD;
 account3.WAVGAPROD = WAVGAPROD;
 account3.WPMAXAPROD = WPMAXAPROD;
 account3.WPAVGAPROD = WPAVGAPROD;
 account3.WMAXAV = WMAXAV;
 account3.WPMAXAV = WPMAXAV;
 memoryfile.write((char*)&account3, sizeof(account3));

 //写入文件
 csv_file << last << ",";
 csv_file << WANUM << ",";
 csv_file << WMAXAPROD << ",";
 csv_file << WAVGAPROD << ",";
 csv_file << WPMAXAPROD << ",";
 csv_file << WPAVGAPROD << ",";
 csv_file << WMAXAV << ",";
 csv_file << WPMAXAV << endl;
                                                                                         
										    
 memoryfile.close();
 csv_file.close();
 file5.close();

 cout<<endl << "last account is: " << last << endl;
 cout<<endl << "The run time is: " << (double)clock() /CLOCKS_PER_SEC<< "s" << endl;
  cout<<"done"<<endl;

}
