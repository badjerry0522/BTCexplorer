#ifndef _CACHE_H
#define _CACHE_H
#include <stdlib.h>
#include <malloc.h>
#include "core_type.h"

enum CACHE_TYPE{NONE,NORMAL,FULL};

#define LINES 4 //4 lines in one set
#define LOG_LINES 2 //log_2(SET_SIZE)
#define LINE_SIZE (4*1024*1024) //line size
#define LOG_LINE_SIZE 22 //log_2(LINE_SIZE)
#define SET_SIZE (LINES*LINE_SIZE)
#define LOG_SET_SIZE (LOG_LINES+LOG_LINE_SIZE)
    
struct line_control{
    uint64_t tag;
    uint8_t valid:1;
    uint8_t dirty:1;
    uint8_t unused:7;
    uint32_t access_times;
    uint8_t *p;
};
struct set_control{
    struct line_control l[LINES];
};
//if d is the power of 2, return 1
//           else         return 0
int ispowerof2(uint64_t d);

//if d is the power of 2  return = log_2(d)
//                 else   return =0 err=INVALID_ARG
int log2(uint64_t d,ERROR_CODE *err);

class cache{
    private:
        struct set_control *sc;
        unsigned char *d;
        uint64_t Csize;     //size of full cache
        int Esize;      //size of element
        int logE;       
        uint64_t set_mask;
        uint64_t tag_mask;
        int set_num;
        int log_set_num;
        uint64_t log_line_size;

        uint64_t max_index;
        char filename[200];
        FILE* fhandle;
        CACHE_TYPE type;

        uint64_t index_set;//set index when load or store is operating
        uint64_t tag;//tag of index when load or store is operating
        uint64_t index_line;//line index when load or store is operating
        int hit;//if hit,0,else -1
        int index_hit;//the line index of the hit line
        int least;//the line to be replaced

        void print_hit(unsigned char *p);

        //copy multi elements to p, with the number of len
        void print_multi_hit(unsigned char *p,int len);

        //if hit, return the NO. of line in the set, else return -1
        int check_is_hit();
        int getleast();

        //for hit_type: 0 for miss, 1 for hit   for dirty: 1 for not wirtten yet, 0 for written
        void update_line(int hit_type,int dirty);
        void write_to_disk(uint64_t index,FILE *fout);
        void write_to_cache(unsigned char *p);

        
        void read_from_disk(uint64_t index,FILE *fout);
        void preparation(uint64_t index);
    public:
        cache();
        //filename: the data filename
        //logC: log_2(C) Memory size in bytes eg. logC=30 means 2^30=1GB
        //logE: log_2(E) element size in bytes eg.  logE=4 means 2^4=16B
        //max_index: the maxium of index
        //Speacial
        //if logC==0 means No cache
        //if 2^logC>2^logE*max_index load all file in the memory
        ERROR_CODE init(char *filename,int logC,int logE,uint64_t _max_index);

        //Load the index=th element to p
        ERROR_CODE load(uint64_t index, unsigned char *p);
        //Load num elements to p
        //ERROR_CODE load(uint64_t index, int num,uint8_t *p);

        //Write the p to the index-th element
        ERROR_CODE store(uint64_t index,unsigned char *p);
        //consecutive elements stored in p, start from index, read them from disk
        ERROR_CODE multiload(uint64_t index,int len,unsigned char *p);


        //get the profile of cache
        void profile(uint64_t *access_time, float miss_ratio);
        ~cache();
};
#endif