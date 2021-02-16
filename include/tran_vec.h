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
		/**
		 * @brief Construct a new tran vec object
		 * 
		 */
		tran_vec();
		
		/**
		 * @brief the number of transactions in this object
		 * 
		 * @return int 
		 */
		int size();
		
		/**
		 * @brief Add one tran in the order of time
		 * 
		 * @param t the seq of transaction, it should greater than the other ones in the object 
		 * @return ERROR_CODE 
		 */
		ERROR_CODE push_back(TRAN_SEQ t);

		
        /**
         * @brief list all tran_seq from the oldest to the last
         * 
         * @return TRAN_SEQ first trna_seq
         */
		TRAN_SEQ begin();

		/**
		 * @brief list all tran_seq accroding to the order
		 * 
		 * @param o FIRST | LAST, FIRST: from the oldest to last, LAST: from the last to the oldest 
		 * @return TRAN_SEQ first tran_seq
		 */
        TRAN_SEQ begin(ORDER o);
        //
        //in spacial order
		/**
		 * @brief list all tran_seq with differenct DIRECTION(BEFORE or AFTER) of the seq
		 * 
		 * @param seq special seq in the object
		 * @param d BEFORE | AFTER 
		 * @param o FIRST | LAST
		 * @return TRAN_SEQ first tran_seq
		 */
		TRAN_SEQ begin(TRAN_SEQ seq,DIRECTION d,ORDER o);

		/**
		 * @brief get one tran_seq according to the lastest begin()
		 * 
		 * @return TRAN_SEQ current tran_seq. If return NULL_SEQ means 
		 */
		TRAN_SEQ next();
                 
		/**
		 * @brief check wether seq is in the object
		 * 
		 * @param seq 
		 * @return int If seq is NOT in the vec return -1; else return the index of it
		 */
		int isIn(TRAN_SEQ seq);
		
		~tran_vec();
};
#endif