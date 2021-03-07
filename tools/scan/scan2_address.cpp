#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <sstream>
#include <jsoncpp/json/json.h>
#include <map>  
#include <vector>
#include <time.h>
//#include <mysql/mysql.h>
using namespace std;

string to_string(int n){

    ostringstream os; //构造一个输出字符串流，流内容为空 
    os << n;         //向输出字符串流中输出int整数i的内容 
    return os.str(); //利用字符串流的str函数获取流中的内容 

}

//交易地址方面的参数
struct addr_info1
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

    addr_info1(){
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
 
int main(int argc, char *argv[]) 
{
    if(argc > 5){
        string path = argv[1];        //交易json文件目录
        string file_num = argv[2];    //交易json文件个数
        string address_num = argv[3]; //地址数量
        string scan2dat = argv[4];    //读取第二次扫描的二进制数据
        string starttxfile = argv[5]; //追加开始的文件编号
        int filenum = atoi(file_num.c_str());
        int addressnum = atoi(address_num.c_str());
        int starttxfileno = atoi(starttxfile.c_str());
        
        //交易地址参数数组
        addr_info1* addrs = new addr_info1[addressnum];
        int txnum = 0;
        
        //创世块：2009-01-04 02:15:05
        int GenesisofBitcoin = 1231006505;

        time_t start_time;  ////起始时间
        start_time = time(NULL);
        
        int nowreadtime = 0;

		//test
		int last = 0;
		ofstream o2;
		o2.open("error.txt");


		//读取第二次扫描的二进制数据
        fstream iofile(scan2dat.c_str(), ios::in | ios::binary);

		iofile.seekg(0, ios::end);
		int64_t data_length = iofile.tellg();
		int64_t size = data_length / sizeof(addr_info1);
		// cout<<size<<endl;
		for (size_t i = 0; i < size; i++)
		{
			iofile.seekg(i * sizeof(addr_info1), ios::beg);
			// iofile.read((char*)&addrs[i+3], sizeof(addr_info1));
			iofile.read((char*)&addrs[i], sizeof(addr_info1));
			cout<<"index:"<<i<<endl;
		}
		iofile.close();
		
		for (unsigned int n = starttxfileno; n < starttxfileno+filenum; n++) {
			ifstream infile;
			string files = path + "/tx_" + to_string(n) + ".txt";
			infile.open(files.data());   
			cout << files << endl;
			//若失败,则输出错误消息
			if (!infile.is_open()) {
				cout << files << endl;
				//return 0;
			}

			string jsonstring;
			while (getline(infile, jsonstring))
			{
				// cout<<jsonstring<<endl;
				Json::Reader reader;
				Json::Value root;

				//解析一个交易json
				 reader.parse(jsonstring, root);
					//读取根节点信息
					//string txhash = root["txhash"].asString();
					//string blockhash = root["txhash"].asString();
					//int fee = root["fee"].asInt();
					int blocktime = ((root["blocktime"].asInt()) - GenesisofBitcoin);
					//test
					//int temp = root["blocktime"].asInt();
					//if (temp > last)
					//{
					//	last = temp;
					//}
					//else
					//{
					//	o2 << n << endl;
					//	o2<< jsonstring <<endl;
					//}
					//cout<<txhash<<endl;
					//cout<<blockhash<<endl;
					//cout<<blocktime<<endl;
					//cout<<fee<<endl;

					//地址类型::   0：非创币交易；1：NonStandardAddress；2：OpReturn；>=3：正常地址
					if (blocktime >= nowreadtime) {
						nowreadtime = blocktime;
						txnum += 1;
						//cout << "tx:" << txnum << endl;
					}
						//交易输入
						vector<int> inputs;
						for (unsigned int i = 0; i < root["inputs"].size(); i++) {
							int address_num = root["inputs"][i]["address"].size();

							//遍历一个交易输入
							for (unsigned int j = 0; j < address_num; j++) {
								int address_id = root["inputs"][i]["address"][j].asInt();

								//正常地址
								if (address_id >= 3) {
										//交易金额
										double v = root["inputs"][i]["value"].asDouble();
										unsigned long long value = uint64_t(v);

										addrs[address_id].Vin += value;
										addrs[address_id].V -= value;
										addrs[address_id].Vmax = addrs[address_id].Vmax > addrs[address_id].V ? addrs[address_id].Vmax : addrs[address_id].V;


										inputs.push_back(address_id);

										//多重签名地址MultisigAddress
										if (address_num > 1) {
											addrs[address_id].Multisig = true;
										}
								}
							}
						}

						//cout<<root["outputs"].size()<<endl;
						//交易输出
						vector<int> outputs;
						for (unsigned int i = 0; i < root["outputs"].size(); i++) {
							int address_num = root["outputs"][i]["address"].size();

							//遍历一个交易输出
							for (unsigned int j = 0; j < address_num; j++) {
								int address_id = root["outputs"][i]["address"][j].asInt();
								//正常地址
								if (address_id >= 3) {
										//交易金额
										double v = root["outputs"][i]["value"].asDouble();
										unsigned long long value = uint64_t(v);

										addrs[address_id].Vout += value;
										addrs[address_id].V += value;
										addrs[address_id].Vmax = addrs[address_id].Vmax > addrs[address_id].V ? addrs[address_id].Vmax : addrs[address_id].V;

										outputs.push_back(address_id);


										//多重签名地址MultisigAddress
										if (address_num > 1) {
											addrs[address_id].Multisig = true;
										}
								}
							}
						}



						//输入
						for (int i = 0; i < inputs.size(); i++) {
							int address_id = inputs[i];

							//第一次出现
							if (addrs[address_id].firsttime == -10) {
								addrs[address_id].firsttime = blocktime;
								//addrs[address_id].lasttime = blocktime;
							}
							if (addrs[address_id].lasttime < blocktime) {
								addrs[address_id].lasttime = blocktime;
							}
							addrs[address_id].K += 1;
							addrs[address_id].Kin += 1;
							//addrs[address_id].Kout;

							addrs[address_id].sumout += outputs.size();

						}

						//输出
						for (int i = 0; i < outputs.size(); i++) {
							int address_id = outputs[i];

							//第一次出现
							if (addrs[address_id].firsttime == -10) {
								addrs[address_id].firsttime = blocktime;
								//addrs[address_id].lasttime = blocktime;
							}
							if (addrs[address_id].lasttime < blocktime) {
								addrs[address_id].lasttime = blocktime;
							}
							addrs[address_id].K += 1;
							addrs[address_id].Kout += 1;


							addrs[address_id].sumin += inputs.size();
						}
					
				
			}

			infile.close();             //关闭文件输入流
		}
		o2.close();

        cout<<"文件读取结束"<<endl;
        
        cout<<"Save addr_info1 addrs[] ............."<<endl;
        
        //csv文件
        ofstream csvFile;
        csvFile.open("data2_address.csv", ios::out); 
        
        //直接保存内存数据（二进制）
        fstream memoryfile("memory2_address.dat", ios::out | ios::binary);
        for(int n = 0; n < addressnum; n++){
            // cout<<"addressNo:"<<n<<endl;
			if (n / 100000000 > 0 && n % 100000000 == 0) cout << "addressNo:" << n << endl;
			
            // 写文件============================================
            csvFile<<n<<',';
            csvFile<<addrs[n].firsttime << ',';
            csvFile<<addrs[n].lasttime << ',';
            csvFile<<addrs[n].K << ',';
            csvFile<<addrs[n].Kin << ',';
            csvFile<<addrs[n].Kout << ',';
            csvFile<<addrs[n].V << ',';
            csvFile<<addrs[n].Vmax << ',';
            csvFile<<addrs[n].Vin <<',';
            csvFile<<addrs[n].Vout <<',';
            csvFile<<addrs[n].Multisig << ',';
            csvFile<<addrs[n].sumout<<',';
            csvFile<<addrs[n].sumin<< endl;
            //===================================================
            memoryfile.write((char*)&addrs[n], sizeof(addrs[n]));

        }
        csvFile.close();
        memoryfile.close();
        
        delete[] addrs;
        
        cout<<"tx:"<<txnum<<endl;

        time_t end_time;  //结束时间
        end_time = time(NULL);
        cout<<"time:"<<end_time - start_time<<endl;
    
    }
    else{
        cout<<"error! Missing required parameter."<<endl;
    }
    
    cout<<"End!"<<endl;
    return 0;
}
