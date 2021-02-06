#include <math.h>
#include <string.h>
#include "../include/core_type.h"
#include <time.h>
using namespace std;

float Satoshi2double(BTC_VOL vol){
    return (float)vol/(float)SATOSHI;
}
float LongSatoshi2float(LONG_BTC_VOL vol){
    return (float)vol/(float)SATOSHI;

}
BTC_VOL float2Satoshi(float vol,ERROR_CODE *err){
    if(vol>MAX_SATOSHI){
        *err=OUT_OF_SATOSHI;
        return 0;
    }
    *err=NO_ERROR;
    float t=vol*(float)SATOSHI;
    BTC_VOL r=(BTC_VOL)t;
    return r;
}
LONG_BTC_VOL float2LongSatoshi(float vol){
    float t=vol*(float)SATOSHI;
    LONG_BTC_VOL r=(BTC_VOL)t;
    return r;
}
CLOCK string2CLOCK(char *str_time){
    struct tm stm;
    strptime(str_time, "%Y-%m-%d %H:%M:%S", &stm);
    CLOCK clock = (CLOCK)mktime(&stm);
    // printf("%d: %s\n", (int)clock, str_time);
    return clock;
}
void CLOCK2string(CLOCK time_stamp,char *str_time){
    time_t tick = (time_t)time_stamp;
    struct tm tm;
    tm = *localtime(&tick);
    strftime(str_time, 20, "%Y-%m-%d %H:%M:%S", &tm);
    // printf("%d: %s\n", (int)tick, str_time);
}
char *error_string(ERROR_CODE err){
    switch(err){
        case INVALID_ARG : return("Invalid Arguments");
        case INVALID_ADDR_SEQ : return("Invalid Address Seq");
        case INVALID_BTC_ADDR: return("Invalid BTC address");
        case OUT_OF_SATOSHI: return("BTC vol is so large");
        case CACHE_OUT_MEMORY: return("Out of memory for cache");
        case CANNOT_OPEN_FILE: return("Can not open file");
        case END_OF_FILE:return("End of file");
        case ERROR_FILE: return("Eorror in file");
    }
    return ("Error?");
}

int btc_hash_6bit(char *btc_addr){
    int len=strlen(btc_addr);
    if(len==0) return 0;

    int pos=len>>1;
    unsigned char c=btc_addr[pos]+btc_addr[pos+1];
    return c&0x3f;
}