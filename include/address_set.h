#ifndef _ADDRESS_SET_H
#define _ADDRESS_SET_H
#include <ostream>
#include <string>
#include "core_type.h"
#include "trans_in_mem.h"
#include "tran_set.h"

using namespace std;
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