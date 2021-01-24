#include <math.h>
#include "../include/core_type.h"
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

}
void CLOCK2string(CLOCK time_stamp,char *str_time){
    
}
