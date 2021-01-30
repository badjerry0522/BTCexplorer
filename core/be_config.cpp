#include <map>
#include <string>
#include <iostream>
#include <ostream>
#include <string.h>
#include "../include/be_config.h"
using namespace std;

be_config::be_config(){
    set_default_value("Address_Dir","./data/address/");
    set_default_value("Account_Dir","./data/account/");
    set_default_value("TIM_Dir","./data/tim/");
    set_default_value("AppWork_Dir","./app_work/");
    set_default_value("Status_File","./BE_Status");
    set_default_value("Progress_File","./BE_Progress");
    set_default_value("Error_File","./BE_Error");
}
void be_config::insert(char *name,struct _config_value *p){
    string namestr;
    namestr.assign(name);
    map<string,struct _config_value *>::iterator iter;
    iter=configs.find(namestr);
    if(iter==configs.end()){
        configs.insert(pair<string, struct _config_value *>(namestr,p));
    }
}
void *be_config::find(char *name){
    string namestr;
    namestr.assign(name);
    map<string,struct _config_value *>::iterator iter;
    iter=configs.find(namestr);
    if(iter!=configs.end()){
        struct _config_value *v=iter->second;
        return v->value;
    }
    cout<<"Bad name "<<name<<endl;
    return NULL;
}

void be_config::set_default_value(char*name,int data){
    struct _config_value *v=(struct _config_value *)malloc(sizeof(struct _config_value));
    v->type=digital;
    v->value=malloc(sizeof(int));
    int *d=(int *)v->value;
    *d=data;
    insert(name,v);
}
void be_config::set_default_value(char*name,char *data){
    struct _config_value *v=(struct _config_value *)malloc(sizeof(struct _config_value));
    v->type=str;
    v->value=malloc(strlen(data)+1);
    char *d=(char *)v->value;
    strcpy(d,data);
    insert(name,v);
}
be_config::~be_config(){

}