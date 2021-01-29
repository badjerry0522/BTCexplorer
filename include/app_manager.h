#ifndef _APP_MANAGER_H
#define _APP_MANAGER_H
#include "be.h"

#define MAX_APP_NUM 100

class app_manager{
    private:
        struct app_record app_list[MAX_APP_NUM];
        int cur_app;
        int app_num;
    public:
        app_manager();
        void add_app(struct app_record *app);
        struct app_record *find_app(char *cmdline,int *argn,void **argv);
        struct app_record *begin();
        struct app_record *next();
};
ERROR_CODE run_app(struct app_record *ar,int argc,void **argv);

#endif