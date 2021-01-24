#ifndef _ADDRESS_SET_H
#define _ADDRESS_SET_H
#include <ostream>
#include <string>
#include "core_type.h"
#include "trans_in_mem.h"
#include "tran_set.h"

using namespace std;


//地址（账户）的信息
struct acc_info
{
	int firsttime;    //最早出现时间
	int lasttime;     //最晚出现时间
	int K;           //参与交易总数
	int Kin;       //存入交易总数
	int Kout;    //取出交易总数
	double V;           //当前账户金额
	double Vmax;  //历史最大金额
	double Vin;      //交易输入金额总数
	double Vout;   //交易输出金额总数
	bool Multisig; //是否为多地址类型
	int sumout;     //取出交易的账户地址总数
	int sumin;        //存入交易的账户地址总数
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
	int ANUM;//地址所属账户中包含的地址数
	int TNUM; //交易的总0数

	acc_info() {
		firsttime = 0;
		lasttime = 0;
		K = 0;
		Kin = 0;
		Kout = 0;
		V = 0.0;
		Vmax = 0.0;
		Vin = 0.0;
		Vout = 0.0;
		Multisig = 0;  //false
		sumout = 0;
		sumin = 0;
		PROD = 0;//活跃时长（单位：秒）
		PRKIKO = 0.0;//输入交易和输出交易之比
		TS = 0.0;//交易频度
		VZERO = 0;//(false)当前金额是否为 0
		VL = 0.0;//交易金额总数和最大金额之比
		T10 = 0;//在 [T1, TK] 时间范围内地址金额为最大金额 10% 以下时间（单位：秒）
		T1050 = 0;//在 [T1, TK] 时间范围内金额为最大金额 10%～50% 时间（单位：秒）
		T50 = 0;//在 [T1, TK] 时间范围内地址金额为最大金额 50% 以上时间（单位：秒）
		PRT10 = 0.0;//地址金额为最大金额 10% 以下时间占比
		PRT1050 = 0.0;//地址金额为最大金额 10%～50% 时间占比
		PRT50 = 0.0;//地址金额为最大金额 50% 以上时间占比
		ARANUM = 0.0;//参与交易的平均地址数
		ANUM  = 0;//地址所属账户中包含的地址数
	}
};





class tran_set;
struct address_set_info{

};
class address_set{
	public:
		//Empty Set
		address_set();
		//One address 
		address_set(string btc_address);
		address_set(ADDR_SEQ addr);
		
		//Multi addresses
		address_set(ADDR_SEQ *p,int num);
		address_set(char *filename);	

		
		//设置和获得标签
		void set_label(LABEL l);
		LABEL get_label(LABEL l);
		
		//设置和获得名字
		void set_name(string name);
		string get_name();
		
		int size();//地址集合中的地址数量
		
		//加入一个地址
		int push_back(string btc_address);
		int push_back(ADDR_SEQ addr);
		
		//判断地址是否在此集合中
		int isIn(string btc_address);
		int isIn(ADDR_SEQ addr);
		
		ADDR_SEQ begin();
		ADDR_SEQ next();
			
		//以下调用依赖于全局交易集合
		CLOCK first_time();//最早出现的时间
		CLOCK last_time();//最晚出现的时间
		uint64_t live_time(); //最晚出现时间-最早出现时间 以秒为单位
		
		BTC_VOL balance();//最终的比特币数
		BTC_VOL balance(CLOCK t);//在时间t的比特币数	
		struct vol_time *balance_time();//比特币金额随着时间变化
		
		tran_set *get_trans(ERROR_CODE *err);
	
		//以下调用依赖于交易集合ts
		tran_set *trans(tran_set *ts);
		CLOCK first_time(tran_set *ts);
		CLOCK last_time(tran_set *ts);
		uint64_t live_time(tran_set *ts); //最晚出现时间-最早出现时间 以秒为单位
		
		int output(ostream outs,int config);
				
		address_set operator+(const address_set &addr_set1);//两个集合相加
		address_set operator-(const address_set &addr_set1);//两个集合相减
		~address_set();
};
#endif