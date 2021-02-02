#ifndef _STATISTICS_H
#define _STATISTICS_H
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core_type.h"

//Valid key: 0,...,limit-1,>=limit
/*Example1
Inputs:
    statistics *s1=new(nothrow) statistics(2,10,"test_stat.txt");

    s1->add(0,1,10);
    s1->add(1,1,10);
    s1->add(1,100,2);
    s1->add(0,0,5);
    s1->add(0,0,10);
    s1->output();
Outputs:
0   15   0.6 0   0 
1   10   0.4 10   0.833333
2   0   0 0   0
3   0   0 0   0
4   0   0 0   0
5   0   0 0   0
6   0   0 0   0
7   0   0 0   0
8   0   0 0   0
9   0   0 0   0
>10   0   0 2   0.166667
*/

class statistics{
    private:
        int *d;
        uint32_t limit;
        uint32_t groups;
        char *title;
        char fname[MAX_FNAME_SIZE]; 
        void _init(uint32_t  groups,uint32_t limit,char *ouput_fname);    
    public:
        statistics(char *output_fname);
        statistics(uint32_t limit,char *output_fname);
        statistics(uint32_t  groups,uint32_t limit,char *ouput_fname);
        void add(uint32_t group,uint32_t key,int value);
        void add(uint32_t key,int value);
        void output();
        ~statistics();
};
#endif