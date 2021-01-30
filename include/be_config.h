#ifndef _BE_CONFIG_H
#define _BE_CONFIG_H
#include <map>
#include <string>
using namespace std;
enum config_value_type{digital,str};

struct _config_value{
    config_value_type type;
    void *value;
};
class be_config{
    private:
        map<string,struct _config_value *> configs;
        void insert(char *name,struct _config_value *value);
    public:
        be_config();
        void  *find(char * name);
        void set_default_value(char *name,int data);
        void set_default_value(char *name,char *data);
        ~be_config();
};
#endif