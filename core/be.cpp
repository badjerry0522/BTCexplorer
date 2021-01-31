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

extern struct app_record graphviz_record;
extern struct app_record split_tran_record;
extern struct app_record select_tran_record;
extern struct app_record addr_of_tran_record;
extern struct app_record analyse_tran_record;
extern struct app_record account_of_addr_record;

ERROR_CODE BEbuildin_bye(int argn,void **argv){
	return EXIT_BE;
}
struct app_record bye_record={"bye","Exit BE",BEbuildin_bye};

ERROR_CODE BEbuildin_help(int argn,void **argv){
	struct BE_env *ep=(struct BE_env *)argv[0];
	app_manager *app_m=ep->app;
	struct app_record *app;
	/*Test for argn and argv
	cout<<argn<<endl;
	if(argn>0){
		for(int i=1;i<=argn;i++)
			printf("%s\n",argv[i]);
	}
	*/
	app=app_m->begin();
	while(app!=NULL){
		printf("%s:\t %s\n",app->cmd,app->help_msg);
//		cout<<app->help_msg<<endl;
		app=app_m->next();
	}
	return NO_ERROR;
}
struct app_record help_record={"help","Show all cmd in BE",BEbuildin_help};


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
	be_env->tim=new trans_in_mem((char *)config->find("TIM_Dir"));
	be_env->accountq=new account_query((char *)config->find("Account_Dir"));
	be_env->addrq=new address_query((char *)config->find("Address_Dir"));
	be_env->app=app;
	be_env->app_work_dir=(char *)config->find("AppWork_Dir");
	be_env->error_fname=(char *)config->find("Error_File");
	be_env->status_fname=(char *)config->find("Status_File");
	be_env->progress_fname=(char *)config->find("Progress_File");

	
	int app_argn;
	void **app_argv=(void **)malloc(sizeof(void *)*(MAX_APP_ARGN+1));
	//init_argv
	app_argv[0]=(void *)be_env;
	//init app_manager
	app->add_app(&help_record);
	app->add_app(&bye_record);
	app->add_app(&graphviz_record);
	app->add_app(&split_tran_record);
	app->add_app(&select_tran_record);
	app->add_app(&addr_of_tran_record);
	app->add_app(&analyse_tran_record);
	app->add_app(&account_of_addr_record);

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
		release_argv(app_argn,app_argv);
		if(ret==EXIT_BE) break;
		if(ret!=NO_ERROR) cout<<error_string(ret)<<endl;
	}
}