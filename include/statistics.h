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
        /**
         * @brief Construct a new statistics object
         * 
         * @param output_fname output file name
         */
        statistics(char *output_fname);
        /**
         * @brief Construct a new statistics object
         * 
         * @param limit upper limit
         * @param output_fname output file name
         */
        statistics(uint32_t limit,char *output_fname);
        /**
         * @brief Construct a new statistics object
         * 
         * @param groups number of groups
         * @param limit  the common limit of groups
         * @param ouput_fname  output file name
         */
        statistics(uint32_t  groups,uint32_t limit,char *ouput_fname);
        /**
         * @brief Add value to the key of the group
         * 
         * @param group 0<=group<groups
         * @param key   0<=key
         * @param value value
         */
        void add(uint32_t group,uint32_t key,int value);
        /**
         * @brief Add value to the key of the group 0
         * 
         * @param key 0<=key
         * @param value value
         */
        void add(uint32_t key,int value);
        /**
         * @brief Ouput the data to the file 
         * 
         */
        void output();
        ~statistics();
};
#endif