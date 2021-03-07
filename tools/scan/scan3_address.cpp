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

//交易地址方面的参数（第二次扫描的数据结构）
struct addr_info0
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

    addr_info0(){
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


//交易地址方面的参数（第三次扫描所需的一些数据，对第二次扫描的数据结构进行合并）
struct addr_info1
{
    int firsttime;   //最早出现时间
    int lasttime;    //最晚出现时间
    int K;           //参与交易总数
    int Kin;         //存入交易总数
    int Kout;        //取出交易总数
	long long V;        //当前账户金额（单位：聪）
	long long Vmax;     //历史最大金额（单位：聪）
	long long VinVout;  //交易金额总数（单位：聪）
    int suminout;    //交易的账户地址总数

    addr_info1(){
        firsttime = 0;
        lasttime = 0;
        K = 0;
        Kin = 0;
        Kout = 0;
        V = 0;
        Vmax = 0;
        VinVout = 0;
        suminout = 0;
    }
};

struct addr_info2{
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
    
    addr_info2(){
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
 
int main(int argc, char *argv[]) 
{
    if(argc > 6){
        string path = argv[1];        //交易json文件目录
        string file_num = argv[2];    //交易json文件数量
        string address_num = argv[3]; //地址数量
        string scan2dat = argv[4];    //第二次扫描的二进制数据
        string scan3dat = argv[5];    //第三次扫描的二进制数据
        string starttxfile = argv[6]; //追加开始的文件编号
        int filenum = atoi(file_num.c_str());
        int addressnum = atoi(address_num.c_str());
        int starttxfileno = atoi(starttxfile.c_str());
        
        addr_info1* addrs = new addr_info1[addressnum];
        cout<<"addr_info1"<<endl;

        addr_info2* addrs2 = new addr_info2[addressnum];
        cout<<"addr_info2"<<endl;

        int txnum = 0;
        
        //创世块：2009-01-04 02:15:05
        int GenesisofBitcoin = 1231006505;

        time_t start_time;  ////起始时间
        start_time = time(NULL);

        addr_info0 addrtemp = addr_info0(); //第二次扫描的数据结构
        
        //读取第二次扫描的二进制数据
        fstream iofile2(scan2dat.c_str(), ios::in | ios::binary);
        for (int i = 0; i < addressnum; i++){
            iofile2.seekg(i * sizeof(addrtemp), ios::beg);
            iofile2.read((char*)&addrtemp, sizeof(addrtemp));
            //cout<<"i:"<<i<<endl;
            addrs[i].lasttime = -10;     //重置最晚出现时间
            addrs[i].V = 0;            //重置当前账户金额

            addrs[i].firsttime = addrtemp.firsttime;    //最早出现时间
            //addrs[i].lasttime;                        //最晚出现时间
            addrs[i].K = addrtemp.K;                    //参与交易总数
            addrs[i].Kin = addrtemp.Kin;                //存入交易总数
            addrs[i].Kout = addrtemp.Kout;              //取出交易总数
            //addrs[i].V;                               //当前账户金额
            addrs[i].Vmax = addrtemp.Vmax;              //历史最大金额
            addrs[i].VinVout = addrtemp.Vin + addrtemp.Vout;         //交易金额总数
            addrs[i].suminout = addrtemp.sumin + addrtemp.sumout;    //交易的账户地址总数
        }
        iofile2.close();


        //读取第二次扫描的二进制数据
        fstream iofile3(scan3dat.c_str(), ios::in | ios::binary);

        iofile3.seekg(0, ios::end);
		int64_t data_length = iofile3.tellg();
		int64_t size = data_length / sizeof(addr_info2);
		// cout<<size<<endl;
		for (size_t i = 0; i < size; i++)
		{
			iofile3.seekg(i * sizeof(addr_info2), ios::beg);
            // iofile3.read((char*)&addrs2[i+3], sizeof(addr_info2));
			iofile3.read((char*)&addrs2[i], sizeof(addr_info2));
			cout<<"index:"<<i<<endl;
		}
		iofile3.close();

		// int index = 0;
		// if (iofile3.is_open())
		// {
		// 	while (!iofile3.eof()){
		// 		iofile3.seekg(index * sizeof(addr_info2), ios::beg);
		// 		iofile3.read((char*)&addrs2[index+3], sizeof(addr_info2));
		// 		index++;
		// 		cout<<"index:"<<index<<endl;
		// 	}
		// }
		// iofile3.close();


        int nowreadtime = 0;

		ofstream err;
		err.open("error.txt", ios::out);



        for(unsigned int n = starttxfileno; n < starttxfileno+filenum; n++){
            ifstream infile;
            string files = path + "/tx_" + to_string(n) +".txt";
            infile.open(files.data());   //将文件流对象与文件连接起来 
            
            //若失败,则输出错误消息,并终止程序运行 
            if( !infile.is_open()){
                 cout<< files <<endl;
                 //return 0;
            }
			cout << files << endl;
            string jsonstring;
            while(getline(infile, jsonstring))
            {
                // cout<<jsonstring<<endl;
                Json::Reader reader;
                Json::Value root;
                
                //解析一个交易json             
                //读取根节点信息
				reader.parse(jsonstring, root);
                    int blocktime = ((root["blocktime"].asInt()) - GenesisofBitcoin);
                     
                    //地址类型::   0：非创币交易；1：NonStandardAddress；2：OpReturn；>=3：正常地址
					if (blocktime >= nowreadtime) {
						nowreadtime = blocktime;
					}
                        //交易输入
                        map<int, unsigned long long> inputs;
                        for(unsigned int i = 0; i < root["inputs"].size(); i++){
                            int address_num = root["inputs"][i]["address"].size();
                            
                            //遍历一个交易输入
                            for(unsigned int j = 0; j< address_num; j++){
                                int address_id = root["inputs"][i]["address"][j].asInt();
                                
                                //正常地址
                                if(address_id >= 3){
                                    if(j == 0){
                                        //交易金额
										// string v = root["inputs"][i]["value"].asString();
										// unsigned long long value = stoll(v);
                                        
                                        double v = root["inputs"][i]["value"].asDouble();
										unsigned long long value = uint64_t(v);


                                        map<int, unsigned long long>::iterator getin;
                                        getin = inputs.find(address_id);
                                        //可能存在同一地址的多个输入
                                        if(inputs.size() == 0 || getin == inputs.end()){
											inputs[address_id] = value;
                                            /*inputs.insert(map<int,unsigned long long>::value_type (address_id, value));*/
                                        }
                                        else{
                                            /*getin->second = getin->second + value;*/
											inputs[address_id] += value;
                                        }
                                    }
                                }
                            }
                        }
                    
                        //cout<<root["outputs"].size()<<endl;
                        //交易输出
                        map<int, unsigned long long> outputs;
                        for(unsigned int i = 0; i < root["outputs"].size(); i++){
                            int address_num = root["outputs"][i]["address"].size();
                            
                            //遍历一个交易输出
                            for(unsigned int j = 0; j< address_num; j++){
                                int address_id = root["outputs"][i]["address"][j].asInt();
                                //正常地址
                                if(address_id >= 3){
                                    if(j == 0){
                                        //交易金额
										// string v = root["outputs"][i]["value"].asString();
										// unsigned long long value = stoll(v);
                                        double v = root["outputs"][i]["value"].asDouble();
										unsigned long long value = uint64_t(v);


                                        map<int, unsigned long long>::iterator getout;
                                        getout = outputs.find(address_id);
                                        //可能存在同一地址的多个输出
                                        if(outputs.size() == 0 || getout == outputs.end()){
                                            /*outputs.insert(map<int, unsigned long long>::value_type (address_id, value));*/
											outputs[address_id] = value;
                                        }
                                        else{
                                            /*getout->second = getout->second + value;*/
											outputs[address_id] += value;
                                        }
                                        
                                    }
                                    
                                }
                            }
                        }
                        
                        
                        map<int, unsigned long long>::iterator iter;  
                        map<int, unsigned long long>::iterator find;  
                        //输入
                        for(iter = inputs.begin(); iter != inputs.end(); iter++){
                            int address_id = iter->first;
                            unsigned long long value_in = iter->second;
                            //cout<<"i:"<<address_id<<"......."<<value_in<<endl;
                            
                            //第一次出现
                            if(addrs[address_id].lasttime == -10){
                                addrs[address_id].lasttime = addrs[address_id].firsttime;
                            }
                            
                            find = outputs.find(address_id);
                            //地址只在输入中
                            if(outputs.size() == 0 || find == outputs.end()){
                                addrs[address_id].V -= value_in;
                                
                                //统计时间
								if (addrs[address_id].lasttime < blocktime) {

									if (addrs[address_id].V < addrs[address_id].Vmax * 0.1 || addrs[address_id].Vmax <= 0) {
										addrs2[address_id].T10 += (blocktime - addrs[address_id].lasttime);
									}
									if (addrs[address_id].V >= addrs[address_id].Vmax * 0.1 && addrs[address_id].V <= addrs[address_id].Vmax * 0.5) {
										addrs2[address_id].T1050 += (blocktime - addrs[address_id].lasttime);
									}
									if (addrs[address_id].V > addrs[address_id].Vmax * 0.5) {
										addrs2[address_id].T50 += (blocktime - addrs[address_id].lasttime);
									}
									addrs[address_id].lasttime = blocktime;
								}
								else if (addrs[address_id].lasttime > blocktime)
								{
									err << n << endl;
									err << jsonstring << endl;

								}
								else
									;
                            }
                            else
                            {
                                //地址既在输入中又在输出中
                                addrs[address_id].V = (addrs[address_id].V - value_in + find->second);
                                
                                //统计时间
								if (addrs[address_id].lasttime < blocktime) {

									if (addrs[address_id].V < addrs[address_id].Vmax * 0.1) {
										addrs2[address_id].T10 += (blocktime - addrs[address_id].lasttime);
									}
									if (addrs[address_id].V >= addrs[address_id].Vmax * 0.1 && addrs[address_id].V <= addrs[address_id].Vmax * 0.5) {
										addrs2[address_id].T1050 += (blocktime - addrs[address_id].lasttime);
									}
									if (addrs[address_id].V > addrs[address_id].Vmax * 0.5) {
										addrs2[address_id].T50 += (blocktime - addrs[address_id].lasttime);
									}
									addrs[address_id].lasttime = blocktime;
								}
								else if (addrs[address_id].lasttime > blocktime)
								{
									err << n << endl;
									err << jsonstring << endl;

								}
								else
									;
                            
                            }
                        
                        }
                        
                        //输出
                        for(iter = outputs.begin(); iter != outputs.end(); iter++){
                            int address_id = iter->first;
                            unsigned long long value_out = iter->second;
                            //cout<<"o:"<<address_id<<"......."<<value_out<<endl;
                            
                            //第一次出现
                            if(addrs[address_id].lasttime == -10){
                                addrs[address_id].lasttime = addrs[address_id].firsttime;
                            }
                     
                            find = inputs.find(address_id);

							//只出现在输出
                            if(inputs.size() == 0 || find == inputs.end()){
                                addrs[address_id].V += value_out;
                                
                                //统计时间
								if (addrs[address_id].lasttime < blocktime) {
									if (addrs[address_id].V < addrs[address_id].Vmax * 0.1) {
										addrs2[address_id].T10 += (blocktime - addrs[address_id].lasttime);
									}
									if (addrs[address_id].V >= addrs[address_id].Vmax * 0.1 && addrs[address_id].V <= addrs[address_id].Vmax * 0.5) {
										addrs2[address_id].T1050 += (blocktime - addrs[address_id].lasttime);
									}
									if (addrs[address_id].V > addrs[address_id].Vmax * 0.5) {
										addrs2[address_id].T50 += (blocktime - addrs[address_id].lasttime);
									}
									addrs[address_id].lasttime = blocktime;
								}
								else if (addrs[address_id].lasttime > blocktime)
								{
									err << n << endl;
									err << jsonstring << endl;

								}
								else
									;
                            }
                        }

              }
            
            infile.close();             //关闭文件输入流
        }

        cout<<"文件读取结束"<<endl;
        
        cout<<"Save data ............."<<endl;
		err.close();
        
        /*
        int PROD;//活跃时长（单位：秒）
        float PRKIKO;//输入交易和输出交易之比
        float TS;//交易频度
        bool VZERO;//当前金额是否为 0
        float VL;//交易金额总数和最大金额之比
        int T10;//在 [T1, TK] 时间范围内地址金额为最大金额 10% 以下时间（单位：秒）
        int T1050;//在 [T1, TK] 时间范围内金额为最大金额 10%～50% 时间（单位：秒）
        int T50;//在 [T1, TK] 时间范围内地址金额为最大金额 50% 以上时间（单位：秒）
        float PRT10;//地址金额为最大金额 10% 以下时间占比
        float PRT1050;//地址金额为最大金额 10%～50% 时间占比
        float PRT50;//地址金额为最大金额 50% 以上时间占比
        float ARANUM;//参与交易的平均地址数
        //int ANUM;//地址所属账户中包含的地址数
        */

       //csv文件
       ofstream csvFile;
       csvFile.open("data3_address.csv", ios::out); 

       //直接保存内存数据（二进制）
       fstream memoryfile("memory3_address.dat", ios::out | ios::binary);

        for(int n = 0; n < addressnum; n++){
			if (n / 100000000 > 0 && n % 100000000 == 0)
					cout << "addressNo:" << n << endl;
            
            
            // 写文件
            csvFile <<n<<',';
            
            //int PROD;//活跃时长（单位：秒）
            addrs2[n].PROD = addrs[n].lasttime - addrs[n].firsttime;
            csvFile<<addrs2[n].PROD<<',';
            
            //double PRKIKO;//输入交易和输出交易之比
            if(addrs[n].Kout == 0){
                addrs2[n].PRKIKO = 1000000.0;
            }
            else{
                addrs2[n].PRKIKO = ((float)addrs[n].Kin / (float)addrs[n].Kout);
            }
            csvFile<<addrs2[n].PRKIKO<<',';
            
            //double TS;//交易频度
            if(addrs2[n].PROD != 0){
                addrs2[n].TS = ((float)addrs[n].K / (float)addrs2[n].PROD);
            }
            csvFile<<addrs2[n].TS<<',';
            
            //bool VZERO;//当前金额是否为 0
            if(addrs[n].V == 0){
                addrs2[n].VZERO = 1;
            }
            else{
                addrs2[n].VZERO = 0;
            }
            csvFile<<addrs2[n].VZERO<<',';
            
            //double VL;//交易金额总数和最大金额之比
            if(addrs[n].Vmax != 0){
                addrs2[n].VL = ((addrs[n].VinVout) / addrs[n].Vmax);
            }
            csvFile<<addrs2[n].VL<<',';
            
            //int T10;//在 [T1, TK] 时间范围内地址金额为最大金额 10% 以下时间（单位：秒）
            csvFile<<addrs2[n].T10<<',';
            
            //int T1050;//在 [T1, TK] 时间范围内金额为最大金额 10%～50% 时间（单位：秒）
            csvFile<<addrs2[n].T1050<<',';
            
            //int T50;//在 [T1, TK] 时间范围内地址金额为最大金额 50% 以上时间（单位：秒）
            csvFile<<addrs2[n].T50<<',';

            //地址只出现一次的情况
            if(addrs2[n].PROD  == 0){
                //double PRT10;//地址金额为最大金额 10% 以下时间占比
                if(addrs[n].V < addrs[n].Vmax * 0.1 || addrs[n].Vmax <= 0){
                    addrs2[n].PRT10 = 1.0;
                }
                //double PRT1050;//地址金额为最大金额 10%～50% 时间占比
                if(addrs[n].V >= addrs[n].Vmax * 0.1 && addrs[n].V <= addrs[n].Vmax * 0.5){
                    addrs2[n].PRT1050 = 1.0;
                }
                //double PRT50;//地址金额为最大金额 50% 以上时间占比
                if(addrs[n].V > addrs[n].Vmax * 0.5){
                    addrs2[n].PRT50 = 1.0;
                }
            }
            else
            {
                //double PRT10;//地址金额为最大金额 10% 以下时间占比
                addrs2[n].PRT10 = ((float)addrs2[n].T10 / (float)addrs2[n].PROD);
                //double PRT1050;//地址金额为最大金额 10%～50% 时间占比
                addrs2[n].PRT1050 = ((float)addrs2[n].T1050 / (float)addrs2[n].PROD);
                //double PRT50;//地址金额为最大金额 50% 以上时间占比
                addrs2[n].PRT50 = ((float)addrs2[n].T50 / (float)addrs2[n].PROD);
            }
            csvFile<<addrs2[n].PRT10<<',';
            csvFile<<addrs2[n].PRT1050<<',';
            csvFile<<addrs2[n].PRT50<<',';
            
            
            //double ARANUM;//参与交易的平均地址数
            if(addrs[n].K != 0){
                addrs2[n].ARANUM = ((float)(addrs[n].suminout) / (float)addrs[n].K);
            }
            csvFile<<addrs2[n].ARANUM<<endl;
            
            //int ANUM;//地址所属账户中包含的地址数
            
            memoryfile.write((char*)&addrs2[n], sizeof(addrs2[n]));

        }
        csvFile.close();
        memoryfile.close();
        
        delete[] addrs;
        delete[] addrs2;
        //delete[] tempdata;

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
