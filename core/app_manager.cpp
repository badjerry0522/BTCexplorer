#include <string.h>
#include "../include/app_manager.h"
app_manager::app_manager(){
    cur_app=0;
    app_num=0;
}

void app_manager::add_app(struct app_record *app){
    memcpy(&(app_list[app_num]),app,sizeof(struct app_record));
    app_num++;
}

struct app_record *app_manager::find_app(char *cmdline,int *argn,void **argv){
    int app_argn=1;
    char *p;
    struct app_record *ap;
    char cmd_name[100];
    p=strtok(cmdline," ");
    if(p!=NULL){
        int found=0;
        strcpy(cmd_name,p);
        for(int i=0;i<app_num;i++){
            ap=&(app_list[i]);
            if(strcmp(cmd_name,ap->cmd)==0){
                found=1;
                break;
            }
        }
        if(found==0) return NULL;
    }
    else{
        return NULL;
    }
    while(p!=NULL){
        p=strtok(NULL," ");
        if(p!=NULL){
            char *arg=(char *)malloc(strlen(p)+1);
            strcpy(arg,p);
            argv[app_argn]=arg;
            app_argn++;
        }
    }
    *argn=app_argn-1;
    return ap;
}

struct app_record *app_manager::begin(){
    if(app_num>0){
        cur_app=1;
//        cout<<"in begin"<<endl;
        return &(app_list[0]);
    }
    return NULL;
}

struct app_record *app_manager::next(){
    if(cur_app<app_num){
        cur_app++;
        return &(app_list[cur_app-1]);
    }
    return NULL;
}