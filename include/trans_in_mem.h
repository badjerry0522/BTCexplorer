#ifndef _TRANS_IN_MEM_H
#define _TRANS_IN_MEM_H
#include <iostream>
#include <ostream>
#include "core_type.h"
#include "tran_set.h"
#include "address_set.h"

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
	uint64_t index;	
};

//data size of one transaction (IN 32 bits WORD)
int tran_data_size(struct tran_info *tp);

struct tran_info2{
	CLOCK time_stamp;
	BTC_VOL fee;
	struct tran_info info;
};
//Only one object in the sysytem
class trans_in_mem{
	public:
		trans_in_mem(char *cache_dir_name);
		
		int load_txt_files(char *dir_name,char *first_file_name,int *err);
		
		//Get the tran_set of one or multi addresses
		tran_set *get_tran_set(ADDR_SEQ seq,ERROR_CODE *err);
		tran_set *get_tran_set(ADDR_SEQ *p,int addr_num,ERROR_CODE *err);
		tran_set *get_tran_set(address_set *as,ERROR_CODE *err);
		
		void get_tran_info(TRAN_SEQ seq,tran_info *ti,ERROR_CODE *err);
		
		void get_inputs(TRAN_SEQ seq,ADDR_SEQ *sp,int *input_num);
		void get_inputs(TRAN_SEQ seq,ADDR_SEQ *sp,LONG_BTC_VOL *bv,int *input_num);
		void get_outputs(TRAN_SEQ seq,ADDR_SEQ *sp,int *input_num);
		void get_outputs(TRAN_SEQ seq,ADDR_SEQ *sp,LONG_BTC_VOL *bv,int *input_num);
		
		void output_tran(TRAN_SEQ seq,int format);
		void output_tran(TRAN_SEQ seq, ostream os, int format);
		void output_tran(tran_set *s,int format);
		void output_tran(tran_set *s, ostream os, int format);
		void output_tran(tran_set *s,ORDER o,int format);
		void output_tran(tran_set *s,ORDER o,ostream os, int format);
		
};
#endif