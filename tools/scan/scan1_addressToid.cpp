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

using namespace std;

string to_string(int n)
{
    ostringstream os; 
    os << n;         
    return os.str();  

}

struct Address
{
    int id;
    string address;
};


int main(int argc, char *argv[]) 
{
    if(argc > 6){
        string path = argv[1];        //原始交易json文件目录
        string file_num = argv[2];    //追加原始交易json文件的个数
        string outpath = argv[3];     //将地址转换为id编号后的交易json文件目录
        string starttxfile = argv[4]; //追加开始的文件编号
        string csvpath = argv[5];     //id-地址csv文件
        string scan1_out = argv[6];   //运行输出目录
        int filenum = atoi(file_num.c_str());
        int starttxfileno = atoi(starttxfile.c_str());

        //创世块：2009-01-04 02:15:05
        int GenesisofBitcoin = 1231006505;

        time_t start_time;  //起始时间
        start_time = time(NULL);
        
        int txnum = 0;
        //地址类型:: 0：coinbase(创币交易)；1：NonStandardAddress；2：OpReturn；>=3：正常地址
        int addressid = 0;
        map<string, int> addressmap;//地址map


        //增加读取address-id文件，进行数据扩充
        if(!csvpath.empty()){
            ifstream fp(csvpath.data());
            string line;
            
            //循环读取每行数据
            while (getline(fp, line)){ 
            
                string idstr;
                string address_str;
                istringstream readstr(line); //string数据流化
                
                //将一行数据按'，'分割
                getline(readstr, idstr, ','); //第一列id
                getline(readstr, address_str, ','); //第二列address

                int id_addr = atoi(idstr.c_str());
                // cout<<id_addr<<endl;
                addressmap.insert(map<string, int>::value_type (address_str, id_addr));
                addressid = addressid > id_addr ? addressid : id_addr;
            }
            addressid++;
            fp.close();
        }

        //地址-id csv文件
        ofstream csvFile;
        csvFile.open(csvpath.c_str(), ios::out|ios::app); // 追加内容

        ofstream newcsvFile;
        string scan1_outfname = scan1_out + "/addressidnew.csv";
        newcsvFile.open(scan1_outfname.data(), ios::out);

        // if(addressmap.empty()){
        //     addressmap.insert(map<string, int>::value_type ("coinbase", 0));
        //     addressmap.insert(map<string, int>::value_type ("NonStandardAddress", 1));
        //     addressmap.insert(map<string, int>::value_type ("NullAddressData", 2));

        //     csvFile<<0<<','<<"coinbase"<<endl;
        //     csvFile<<1<<','<<"NonStandardAddress"<<endl;
        //     csvFile<<2<<','<<"NullAddressData"<<endl;
        // }

        cout<<"addressmap:"<<addressmap.size()<<endl;

        for(unsigned int n = starttxfileno; n < starttxfileno+filenum; n++){
            ifstream infile; 
            string files = path + "/tx_" + to_string(n) +".txt";
            infile.open(files.data());

            ofstream outFile;
            string outfiles = outpath + "/tx_" + to_string(n) +".txt";
            outFile.open(outfiles.data(), ios::out);
            
            //若失败,则输出错误消息,并终止程序运行 
            if( !infile.is_open()){
                cout<<"error:"<< files <<endl;
                //return 0;
            }
            
            cout<<"do:"<<files<<endl;
            string jsonstring;
            while(getline(infile, jsonstring))
            {
                //cout<<jsonstring<<endl;
                Json::Reader reader;
                Json::Value root;

                Json::Value newroot;
                    
                //解析一个交易json
                if (reader.parse(jsonstring, root)){
                    //读取根节点信息
                    // newroot["txhash"] = Json::Value(root["txhash"].asString());
                    // newroot["fee"] = Json::Value(root["fee"].asDouble());
                    newroot["blocktime"] = Json::Value(root["blocktime"].asInt());

                    //int blocktime = ((root["blocktime"].asInt()) - GenesisofBitcoin);
                    //cout<<txhash<<endl;
                    //cout<<blocktime<<endl;
                    //cout<<fee<<endl;

                    //地址类型:: 0：coinbase(创币交易)；1：NonStandardAddress；2：OpReturn；>=3：正常地址

                    txnum += 1;
                    // cout<<"tx:"<<txnum<<endl;

                    // newroot["fee"] = Json::Value(stoll(root["fee"].asString()));
                    double fee = root["fee"].asDouble();
                    newroot["fee"] = Json::Value(Json::Value::Int64(fee));
                
                    //交易输入
                    Json::Value inputs;
                    for(unsigned int i = 0; i < root["inputs"].size(); i++){
                        //一个交易输入的地址数组大小（存在多重签名）
                        int address_num = root["inputs"][i]["address"].size();

                        //交易输入
                        Json::Value oneinput;
                        //交易输入地址数组
                        Json::Value addresslist;
                            
                        //遍历一个交易输入
                        for(unsigned int j = 0; j < address_num; j++){
                            string address = root["inputs"][i]["address"][j].asString();

                            if(address.compare("coinbase") == 0){
                                addresslist.append(0);
                            }
                            else if(address.compare("NonStandardAddress") == 0){
                                addresslist.append(1);
                            }
                            else if(address.compare("NullAddressData") == 0){
                                addresslist.append(2);
                            }
                            else{
                                map<string, int>::iterator l_it;
                                l_it = addressmap.find(address);

                                //新的地址id
                                if(l_it == addressmap.end()){
                                    //id = addressid;
                                    addresslist.append(addressid);
                                    addressmap.insert(map<string, int>::value_type (address, addressid));

                                    csvFile<<addressid<<',';   //id
                                    csvFile<<address<<endl;   //address

                                    newcsvFile<<addressid<<',';   //id
                                    newcsvFile<<address<<endl;   //address

                                    addressid++;
                                }
                                else{
                                    //id = l_it->second;
                                    addresslist.append(l_it->second);
                                }
                            }
                        }

                        //输入金额
                        // oneinput["value"] = Json::Value(stoll(root["inputs"][i]["value"].asString()));
                        double value = root["inputs"][i]["value"].asDouble();
                        oneinput["value"] = Json::Value(Json::Value::Int64(value));
                        oneinput["address"] = Json::Value(addresslist);

                        //一个交易输入
                        inputs.append(Json::Value(oneinput));
                    }
                    newroot["inputs"] = Json::Value(inputs);


                    //交易输出
                    Json::Value outputs;
                    for(unsigned int i = 0; i < root["outputs"].size(); i++){
                        //一个交易输出的地址数组大小（存在多重签名）
                        int address_num = root["outputs"][i]["address"].size();

                        //交易输出
                        Json::Value oneoutput;
                        //交易输出地址数组
                        Json::Value addresslist;
                            
                        //遍历一个交易输出
                        for(unsigned int j = 0; j< address_num; j++){
                            string address = root["outputs"][i]["address"][j].asString();

                            if(address.compare("NonStandardAddress") == 0){
                                addresslist.append(1);
                            }
                            else if(address.compare("NullAddressData") == 0){
                                addresslist.append(2);
                            }
                            else{
                                map<string, int>::iterator l_it;
                                l_it = addressmap.find(address);

                                //新的地址id
                                if(l_it == addressmap.end()){
                                    //id = addressid;
                                    addresslist.append(addressid);
                                    addressmap.insert(map<string, int>::value_type (address, addressid));

                                    csvFile<<addressid<<',';   //id
                                    csvFile<<address<< endl;   //address

                                    newcsvFile<<addressid<<',';   //id
                                    newcsvFile<<address<< endl;   //address
                                    
                                    addressid++;
                                }
                                else{
                                    //id = l_it->second;
                                    addresslist.append(l_it->second);
                                }
                            }
                        }

                        //输出金额
                        // oneoutput["value"] = Json::Value(stoll(root["outputs"][i]["value"].asString()));
                        double value = root["outputs"][i]["value"].asDouble();
                        oneoutput["value"] = Json::Value(Json::Value::Int64(value));
                        oneoutput["address"] = Json::Value(addresslist);

                        //一个交易输出
                        outputs.append(Json::Value(oneoutput));
                    }
                    newroot["outputs"] = Json::Value(outputs);
                    //string newjson = newroot.toStyledString();  //将Json对象序列化为字符串

                    newroot["txhash"] = Json::Value(root["txhash"].asString());

                    Json::FastWriter fast_writer;
                    string newjson = fast_writer.write(newroot);
                    //cout<<newjson<<endl;
                    outFile<<newjson;
                }
            }

            infile.close();             //关闭文件输入流
            outFile.close();
        }

        cout<<"文件读取结束"<<endl;
        cout<<"Save addrs[] ............."<<endl;
        cout<<"txnum:"<<txnum<<endl;
        cout<<"addressmap:"<<addressmap.size()<<endl;

        
        // //地址-id csv文件
        // ofstream csvFile;
        // csvFile.open("addressid.csv", ios::out); 

        // map<string, int>::iterator iter;
        // for(iter = addressmap.begin(); iter != addressmap.end(); iter++){
        //     //cout<<n<<endl;
        //     //写文件
        //     csvFile<<iter->second<<',';   //id
        //     csvFile<<iter->first<< endl;  //address
        // }
        // csvFile.close();

        csvFile.close();
        newcsvFile.close();
        
        int64_t addressmap_size = addressmap.size();
        addressmap.clear();

        time_t end_time;//结束时间
        end_time = time(NULL);
        cout<<"time:"<<end_time - start_time<<endl;

        ofstream oFile;
        string metafname = scan1_out + "/scan1_addr_meta.txt";
        oFile.open(metafname.data(), ios::out);
        oFile<<(end_time - start_time)<<" "<<txnum<<" "<<addressmap_size<<endl;
        oFile.close();
        
        

    }
    else{
        cout<<"error! Missing required parameter."<<endl;
    }
    cout<<"End!"<<endl;
    return 0;

}
