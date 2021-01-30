#ifndef _CORE_TYPE_H
#define _CORE_TYPE_H
#include <stdint.h>
#define MAX_INPUT_NUM 1<<16
#define MAX_OUTPUT_NUM 1<<16
#define MAX_BTC_ADDRESS_LEN 42

//Special addresses
#define COINBASE_SEQ 0xFFFFFFFF //for coinbase   -1
#define NonStandardAddress 0xFFFFFFFE //non standard address -2
#define OpReturn 0xFFFFFFFD //BlockSci opreturn -3
#define NULL_SEQ 0xFFFFFFFC //NULL SEQ

typedef uint32_t BLOCK_SEQ; 
typedef uint32_t ADDR_SEQ;
typedef uint32_t TRAN_SEQ;
typedef uint32_t ACCOUNT_SEQ;

typedef uint64_t CLOCK;

typedef uint32_t BTC_VOL;//btc volume in Satoshi
typedef uint64_t LONG_BTC_VOL;//long btc volume

#define SATOSHI 1000000000
#define MAX_SATOSHI 4.294967295
#define MAX_FNAME_SIZE 256

enum ORDER{FIRST,LAST};//order of time

enum ADDR_TYPE{P2PKH,P2PSH,SegWit};//types of btc address

enum LABEL{UNKNOWN,EXCHANGES,POOLS,GAMBLING,MIXER, MARKET};

enum ERROR_CODE{NO_ERROR,
    EXIT_BE,
    INVALID_ARG,    //invalid argument
    INVALID_ADDR_SEQ,
    INVALID_BTC_ADDR,
    OUT_OF_SATOSHI,
    CACHE_OUT_MEMORY,
    CANNOT_OPEN_FILE,
    END_OF_FILE,
    ERROR_FILE};

float Satoshi2float(BTC_VOL vol);
float LongSatoshi2float(LONG_BTC_VOL vol);
BTC_VOL float2Satoshi(float vol,ERROR_CODE *err);
LONG_BTC_VOL float2LongSatoshi(float vol);
CLOCK string2CLOCK(char *str_time);
void CLOCK2string(CLOCK time_stamp,char *str_time);
char *error_string(ERROR_CODE err);
#endif
