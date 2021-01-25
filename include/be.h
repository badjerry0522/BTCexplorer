#ifndef _BE_H
#define _BE_H

#include <iostream>   

#include "account_query.h"
#include "account.h"
#include "address_query.h"
#include "address_set.h"
#include "core_type.h"
#include "trans_in_mem.h"
#include "app_manager.h"

using namespace std;

class app_manager;

class address_query;
class account_query;
class trans_in_mem;
struct app_record{
    char*cmd;
    char *help_msg;          //app help massage name
    int (*app_fun)(int,void **); //application function
};
struct BE_env{
   address_query *addrq;
   account_query *accountq;
   trans_in_mem *tim;
   app_manager *app;
   ostream status;
   ostream progress;
   ostream error;
   istream keyboard; 
};

#endif