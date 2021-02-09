#include <iostream>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/core_type.h"
#include "../include/trans_in_mem.h"
#include "../include/account.h"
#include "../include/address_query.h"
#include "../include/address_set.h"
#include "../include/tran_set.h"
#include "../include/app_manager.h"
#include "../include/be.h"
#include "../include/be_config.h"

using namespace std;
class trans_in_mem;
class account_query;
class address_query;

#define MAX_APP_ARGN 10


void release_argv(int app_argn,void **app_argv){
	for(int i=1;i<app_argn;i++){
		free(app_argv[i]);
	}
}
ERROR_CODE run_app(struct app_record *ap,int app_argn,void **app_argv){
	ERROR_CODE ret;
	ret=ap->app_fun(app_argn,app_argv);
	return ret;
}
int main(int argn,char **argv){
	be_config *config=new be_config();
	app_manager *app=new app_manager();
	struct BE_env *be_env=(struct BE_env *)malloc(sizeof(struct BE_env));
	//init be_env
	//be_env->tim=new trans_in_mem((char *)config->find("TIM_Dir"));
	be_env->accountq=new account_query((char *)config->find("Account_Dir"));
	be_env->addrq=new address_query((char *)config->find("Address_Dir"));
	be_env->app=app;
	be_env->app_work_dir=(char *)config->find("AppWork_Dir");
	be_env->error_fname=(char *)config->find("Error_File");
	be_env->status_fname=(char *)config->find("Status_File");
	be_env->progress_fname=(char *)config->find("Progress_File");
	be_env->screen=new ostringstream;
	
	int app_argn;
	void **app_argv=(void **)malloc(sizeof(void *)*(MAX_APP_ARGN+1));
	//init_argv
	app_argv[0]=(void *)be_env;
	//init app_manager

	add_app(app);
	be_env->screen->seekp(ios::beg);
	char cmdline[1025];
	while(1){
		cout<<"%";
		
		cin.getline(cmdline,1023);
		struct app_record *cur_app;
		cur_app=app->find_app(cmdline,&app_argn,app_argv);
		if(cur_app==NULL){
			cout<<"Can not find this command"<<endl;
			continue;
		}
		ERROR_CODE ret;
		ret=run_app(cur_app,app_argn,app_argv);
		cout<<be_env->screen->str();
		be_env->screen->str("");
		release_argv(app_argn,app_argv);
		if(ret==EXIT_BE) break;
		if(ret!=NO_ERROR) cout<<error_string(ret)<<endl;
	}
}