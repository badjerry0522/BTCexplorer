#ifndef _CACHE_H
#define _CACHE_H
#include <stdlib.h>
#include <malloc.h>
#include "core_type.h"

#define set_size 4 //4 lines in one set

//if d is the power of 2  return = log_2(d)
//                 else   return =0 err=INVALID_ARG
int log2(uint64_t d,ERROR_CODE *err);

class cache{
    public:
        cache();
        //filename: the data filename
        //logC: log_2(C) Memory size in bytes eg. logC=9 means 2^9=1GB
        //logE: log_2(E) element size in bytes eg.  logE=4 means 2^4=16B
        //max_index: the maxium of index
        //Speacial
        //if logC==0 means No cache
        //if 2^logC>2^logE*max_index load all file in the memory
        ERROR_CODE init(char *filename,int logC,int logE,uint64_t max_index);

        //Load the index=th element to p
        ERROR_CODE load(uint64_t index, uint8_t *p);
        //Load num elements to p
        //ERROR_CODE load(uint64_t index, int num,uint8_t *p);

        //get the profile of cache
        void profile(uint64_t *access_time, float miss_ratio);
        ~cache();
};
#endif