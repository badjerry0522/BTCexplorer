#ifndef _TRANS_IN_MEM_H
#define _TRANS_IN_MEM_H
#include <iostream>
#include <ostream>
#include "core_type.h"
#include "tran_set.h"
#include "address_set.h"
#include "cache.h"
#include "tran_vec.h"
class tran_set;
struct block_info{
	CLOCK block_time;
	TRAN_SEQ first_tran_seq;
	int tran_num;
};

struct tran_info{
	uint16_t input_num;	
	uint16_t output_num;
	uint16_t long_btc_vol;//=0,or 1
	uint16_t unused;
	uint64_t index;	//index of uint32_t
};
struct addr_tran_binary{
	ADDR_SEQ addr;
	LONG_BTC_VOL bitcoin;
};
struct transaction_binary{
	CLOCK block_time;
	struct addr_tran_binary inputs[MAX_INPUT_NUM];
	struct addr_tran_binary outputs[MAX_OUTPUT_NUM];
	int valid_inputs;
	int valid_outputs;
};

//data size of one transaction (IN 32 bits WORD)
int tran_data_size(struct tran_info *tp);

//Only one object in the sysytem
class trans_in_mem{
	private:
		char *dir_name;
		cache ca_block,ca_av,ca_trans,ca_addr_show_times,ca_addr_is_count;
		uint64_t max_addr,num_trans,av_size;
		char des_av[100],des_TIM_meta[100],des_trans[100],des_addr_show_times[100],des_addr_is_count[100];
	public:
		trans_in_mem(char *dir_name);
		
		//Fill ti
		ERROR_CODE get_tran_binary(TRAN_SEQ seq,struct transaction_binary *ti);
		
		//The number of input and output address
		int get_input_num(TRAN_SEQ seq,ERROR_CODE *err);
		int get_output_num(TRAN_SEQ seq,ERROR_CODE *err);
		
		//The bitcoin vol. of input and output
		LONG_BTC_VOL get_input_bitcion(TRAN_SEQ seq, ERROR_CODE *err);
		LONG_BTC_VOL get_output_bitcion(TRAN_SEQ seq, ERROR_CODE *err);
		
		//The block time of transanction
		CLOCK get_block_time(TRAN_SEQ seq,ERROR_CODE *err);

		//Translation bwteen binary and text format
		ERROR_CODE binary2text(struct transaction_binary *pb,struct transaction *pt);
		ERROR_CODE text2binary(struct transaction *pt,struct transaction_binary *pb);

		//Number of transactions with seq
		int tran_num(ADDR_SEQ seq, ERROR_CODE *err);
		//Get the set of transaction with seq
		tran_vec *get_tran_set(ADDR_SEQ seq,ERROR_CODE *err);

		//Max seq of addr,tran and block
		ADDR_SEQ max_addr_seq();
		TRAN_SEQ max_tran_seq();
		BLOCK_SEQ max_block_seq();

		//CLOCK time of the first block and last block
		CLOCK first_block_time();
		CLOCK last_block_time();
};
#endif