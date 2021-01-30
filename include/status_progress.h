#ifndef _STATUS_PROGRESS_H
#define _STATUS_PORGRESS_H
#include <stdint.h>
#include <sys/time.h>
#include <fstream>

using namespace std;
class status{
    private:
        char file_name[256];
        ofstream f;
    public:
        status(char *filename);
        void report(char *info);
        ~status();
};
class progress{
    private:
        char file_name[256];
        uint64_t max;
        int last_value;
        struct timeval last_time;  
    public:
        progress(char *fname,uint64_t _max);
        void value(uint64_t v);
        ~progress();

};
#endif