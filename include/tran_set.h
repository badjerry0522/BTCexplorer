#ifndef _TRAN_SET_H
#define _TRAN_SET_H
#include <iostream>
#include <string>
#include <set>
#include "core_type.h"
#include "trans_in_mem.h"
#include "address_set.h"
using namespace std;
class address_set;
class trans_in_mem;

class tran_set{
	public:
		//Empty Set
		tran_set(trans_in_mem *g);
		//With one tran
		tran_set(trans_in_mem *g,TRAN_SEQ seq);
		
		//tran_set(string filename);
		
		//Number of set
		int size();
		//Add one tran
		int push_back(TRAN_SEQ t);
		
		
		TRAN_SEQ begin(ORDER o);
		TRAN_SEQ next();
		

		int isIn(TRAN_SEQ seq);
		CLOCK first_time();
		CLOCK last_time();
		uint64_t live_time(); 
		
		tran_set *filter(ADDR_SEQ seq);

		tran_set *filter(address_set *as);

		tran_set *filter(struct tm *first_time,struct tm *last_time);
		
		tran_set operator+(const tran_set &tran_set1);
		tran_set operator-(const tran_set &tran_set1);
		int output(ostream outs,int config);
		~tran_set();
};
#endif