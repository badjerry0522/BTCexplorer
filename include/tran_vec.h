#ifndef _TRAN_VEC_H
#define _TRAN_VEC_H
#include <vector>

#include "core_type.h"
#include "trans_in_mem.h"
#include "address_set.h"
using namespace std;
class address_set;
class trans_in_mem;

class tran_vec{
    private:
        vector<TRAN_SEQ> tv;
        int cur=0;
        int last_index;
        int step=0;
	public:
		//Empty Set
		tran_vec();
		
		//Number of set
		int size();
		//Add one tran in the order of time
		ERROR_CODE push_back(TRAN_SEQ t);
		//list all tran_seq from the oldest to the last
        TRAN_SEQ begin();
        TRAN_SEQ begin(ORDER o);
        //list all tran_seq with differenct DIRECTION(BEFORE or AFTER) of the t
        //in spacial order
		TRAN_SEQ begin(TRAN_SEQ seq,DIRECTION d,ORDER o);
		TRAN_SEQ next();

        //If seq is NOT in the vec return -1
        //                    else return the index of it
		int isIn(TRAN_SEQ seq);
		
		~tran_vec();
};
#endif