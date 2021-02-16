#ifndef _BLOCK_FILE_H
#define _BLOCK_FILE_H

#include "core_type.h"
#include "trans_in_mem.h"

/*INPUT:
buf: the string holding one transaction
OUTPUTS:
1. block_time
2. fill struct tran_info: tp, except index
3. The layout of addr:
ADDR_SEQ input_address[input_num]
ADDR_SEQ output_address[output_num]
4. The layout of vol:
if long_btc_vol==1
LONG_BTC_VOL input_vol[input_num]
LONG_BTC_VOL output_vol[output_num]
else
BTC_VOL input_vol[input_num]
BTC_VOL output_vol[output_num]
*/
ERROR_CODE parse_tran(char *buf,CLOCK *block_time,struct tran_info *tp, 
              ADDR_SEQ *addr, uint64_t *vol);

#endif