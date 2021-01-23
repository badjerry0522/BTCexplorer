#include "core_type.h"
#include "trans_in_mem.h"
int tran_data_size(struct tran_info *tp){
	return (sizeof(ADDR_SEQ)+sizeof(BTC_VOL)<<tp->long_btc_vol)*(tp->input_num+tp->output_num);
}