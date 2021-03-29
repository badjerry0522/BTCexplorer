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

#define MAX_BTC_ADDRESS_LEN 42
typedef int32_t BLOCK_SEQ; 
typedef int32_t ADDR_SEQ;
typedef int32_t TRAN_SEQ;
typedef int32_t ACCOUNT_SEQ;
typedef int64_t CLOCK;

enum ADDR_TYPE{P2PKH,P2PSH,SegWit};//types of btc address 1\3\bc1 (https://www.sohu.com/a/295681909_120061189)

struct address_info2{
    int tran_num;	
    BLOCK_SEQ first_block_seq,last_block_seq;
    CLOCK first_clock,last_clock;
    int64_t balance;

    address_info2(){
        tran_num = 0;
        first_block_seq = -10;
        last_block_seq = -10;
        first_clock = -10;
        last_clock = -10;
        balance = 0;
    }
};

struct address_info{
    ADDR_SEQ seq;
    char btc_address[MAX_BTC_ADDRESS_LEN];  		//seq---btc_address table
    ADDR_TYPE type;      
    struct address_info2 info2;	//seq--info2 table
    ACCOUNT_SEQ account_seq; 	//seq--account table
    int valid;
};
 
int main(int argc, char *argv[]) 
{
    if(argc > 4){
        string path = argv[1];        //交易json文件目录
        string file_num = argv[2];    //交易json文件数量
        string meta = argv[3];        //meta文件
        string outpath = argv[4];
        int filenum = atoi(file_num.c_str());
        int addressnum;
		ifstream f;
		string line;
		f.open(meta);
		while (getline(f, line))
		{
			int time, acc_num;
			stringstream input(line);
			input >> time >> acc_num >> addressnum;
		}
		f.close();



        // string files[filenum];  //输入文件列表
        // for(unsigned int n = 0; n < filenum; n++){
        //     files[n] = path + "/tx_" + to_string(n) +".txt";
        //     //cout<< files[n] <<endl;
        // }
        
        //交易地址参数数组
        address_info2* addrs = new address_info2[addressnum];
        int txnum = 0;
        
        //创世块：2009-01-04 02:15:05
        int GenesisofBitcoin = 1231006505;

        time_t start_time;  ////起始时间
        start_time = time(NULL);
        
        int pblocktime = GenesisofBitcoin;  //上一个区块的时间
        int blocknum = 0;

        for(unsigned int n = 0; n < filenum; n++){
            ifstream infile; 
            string files = path + "/tx_" + to_string(n) +".txt";
            infile.open(files.data());   //将文件流对象与文件连接起来 
            
            //若失败,则输出错误消息,并终止程序运行 
            if( !infile.is_open()){
                cout<<"error:"<< files <<endl;
            }
            cout<< "do:" << files << endl;
            string jsonstring;
            while(getline(infile, jsonstring))
            {
                // cout<<jsonstring<<endl;
                Json::Reader reader;
                Json::Value root;
                
                //解析一个交易json
                if (reader.parse(jsonstring, root)){
                    //读取根节点信息
                    //string txhash = root["txhash"].asString();
                    //string blockhash = root["txhash"].asString();
                    //int fee = root["fee"].asInt();
                    int blocktime = (int)root["blocktime"].asInt();
                    
                    //cout<<txhash<<endl;
                    //cout<<blockhash<<endl;
                    //cout<<blocktime<<endl;
                    //cout<<fee<<endl;

                    //地址类型:: 0：coinbase(创币交易)；1：NonStandardAddress；2：OpReturn；>=3：正常地址
                    if(blocktime >= pblocktime){

                        txnum += 1;
                        //cout<<"tx:"<<txnum<<endl;

                        if(blocktime != pblocktime){
                            pblocktime = blocktime;
                            blocknum++;
                        }
                
                        //交易输入
                        map<int, int64_t> inputs;
                        for(unsigned int i = 0; i < root["inputs"].size(); i++){
                            int address_num = root["inputs"][i]["address"].size();
                            
                            //遍历一个交易输入
                            for(unsigned int j = 0; j< address_num; j++){
                                int address_id = root["inputs"][i]["address"][j].asInt();
                                
                                //正常地址
                                if(address_id >= 0){
                                    if(j == 0){
                                        //交易金额
                                        int64_t value = (int64_t)root["inputs"][i]["value"].asDouble();

                                        map<int, int64_t>::iterator getin;
                                        getin = inputs.find(address_id);
                                        //可能存在同一地址的多个输入
                                        if(inputs.size() == 0 || getin == inputs.end()){
                                            inputs.insert(map<int, int64_t>::value_type (address_id, value));
                                        }
                                        else{
                                            getin->second = getin->second + value;
                                        }
                                    }
                                }
                            }
                        }
                    
                        //cout<<root["outputs"].size()<<endl;
                        //交易输出
                        map<int, int64_t> outputs;
                        for(unsigned int i = 0; i < root["outputs"].size(); i++){
                            int address_num = root["outputs"][i]["address"].size();
                            
                            //遍历一个交易输出
                            for(unsigned int j = 0; j< address_num; j++){
                                int address_id = root["outputs"][i]["address"][j].asInt();
                                //正常地址
                                if(address_id >= 0){
                                    if(j == 0){
                                        //交易金额
                                        int64_t value = (int64_t)root["outputs"][i]["value"].asDouble();

                                        map<int, int64_t>::iterator getout;
                                        getout = outputs.find(address_id);
                                        //可能存在同一地址的多个输出
                                        if(outputs.size() == 0 || getout == outputs.end()){
                                            outputs.insert(map<int, int64_t>::value_type (address_id, value));
                                        }
                                        else{
                                            getout->second = getout->second + value;
                                        }
                                    }
                                }
                            }
                        }


                        map<int, int64_t>::iterator iter;  
                        map<int, int64_t>::iterator find;  
                        //输入
                        for(iter = inputs.begin(); iter != inputs.end(); iter++){
                            int address_id = iter->first;
                            int64_t value_in = iter->second;
                            //cout<<"i:"<<address_id<<"......."<<value_in<<endl;
                            
                            find = outputs.find(address_id);
                            //地址只在输入中
                            if(outputs.size() == 0 || find == outputs.end()){
                                addrs[address_id].balance -= value_in;
                                
                                //第一次出现
                                if(addrs[address_id].first_clock == -10){
                                    addrs[address_id].first_clock = blocktime;
                                }
                                if(addrs[address_id].last_clock < blocktime){
                                    addrs[address_id].last_clock = blocktime;
                                }

                                if(addrs[address_id].first_block_seq == -10){
                                    addrs[address_id].first_block_seq = blocknum;
                                }
                                if(addrs[address_id].last_block_seq < blocknum){
                                    addrs[address_id].last_block_seq = blocknum;
                                }
                                addrs[address_id].tran_num++;
                            }
                            else
                            {
                                //地址既在输入中又在输出中
                                addrs[address_id].balance = (addrs[address_id].balance - value_in + find->second);
                                
                                //第一次出现
                                if(addrs[address_id].first_clock == -10){
                                    addrs[address_id].first_clock = blocktime;
                                }
                                if(addrs[address_id].last_clock < blocktime){
                                    addrs[address_id].last_clock = blocktime;
                                }

                                if(addrs[address_id].first_block_seq == -10){
                                    addrs[address_id].first_block_seq = blocknum;
                                }
                                if(addrs[address_id].last_block_seq < blocknum){
                                    addrs[address_id].last_block_seq = blocknum;
                                }
                                addrs[address_id].tran_num++;
                            }
                            
                        }
                    
                        //输出
                        for(iter = outputs.begin(); iter != outputs.end(); iter++){
                            int address_id = iter->first;
                            int64_t value_out = iter->second;
                            //cout<<"o:"<<address_id<<"......."<<value_out<<endl;
                            
                            find = inputs.find(address_id);
                            if(inputs.size() == 0 || find == inputs.end()){
                                addrs[address_id].balance += value_out;
                                
                                //第一次出现
                                if(addrs[address_id].first_clock == -10){
                                    addrs[address_id].first_clock = blocktime;
                                }
                                if(addrs[address_id].last_clock < blocktime){
                                    addrs[address_id].last_clock = blocktime;
                                }

                                if(addrs[address_id].first_block_seq == -10){
                                    addrs[address_id].first_block_seq = blocknum;
                                }
                                if(addrs[address_id].last_block_seq < blocknum){
                                    addrs[address_id].last_block_seq = blocknum;
                                }
                                addrs[address_id].tran_num++;
                            }
                        }
                    }
                }
            }
        
            infile.close();             //关闭文件输入流
        }

        cout<<"文件读取结束"<<endl;
        
        cout<<"Save addr_info1 addrs[] ............."<<endl;
        
        //直接保存内存数据（二进制）
        string address_info2fname = outpath + "/address_info2.dat";
        fstream memoryfile(address_info2fname.data(), ios::out | ios::binary);
        for(int n = 0; n < addressnum; n++){
            if (n / 100000000 > 0 && n % 100000000 == 0)
			{
				cout<<"addressNo:"<<n<<endl;
			}
            memoryfile.write((char*)&addrs[n], sizeof(addrs[n]));
        }
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
