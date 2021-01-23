#ifndef _CORE_TYPE_H
#define _CORE_TYPE_H

#define MAX_INPUT_NUM 1<<16
#define MAX_OUTPUT_NUM 1<<16
#define MAX_BTC_ADDRESS_LEN 42

#define NULL_SEQ 0xFFFFFFFE //NULL SEQ
#define COINBASE_SEQ 0xFFFFFFFF //for coinbase
#include<stdint.h>
typedef uint32_t BLOCK_SEQ; 
typedef uint32_t ADDR_SEQ;
typedef uint32_t TRAN_SEQ;
typedef uint32_t ACCOUNT_SEQ;

typedef uint64_t CLOCK;

typedef uint32_t BTC_VOL;//btc volume in Satoshi
typedef uint64_t LONG_BTC_VOL;//long btc volume

enum ORDER{FIRST,LAST};//order of time

enum ADDR_TYPE{P2PKH,P2PSH,SegWit};//types of btc address

enum LABEL{UNKNOWN,EXCHANGES,POOLS,GAMBLING,MIXER, MARKET};

enum ERROR_CODE{NO_ERROR,INVALID_ADDR_SEQ,INVALID_BTC_ADDR};

#endif
