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
using namespace std;
#define MAX_APP_ARGN 10

extern struct app_record graphviz_record;
extern struct app_record split_tran_record;
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
int run_app(struct app_record *ap,int app_argn,void **app_argv){
	return ap->app_fun(app_argn,app_argv);
}
int main(int argn,char **argv){
	if(argn!=2){
		cout<<"Usage: be BE_DIR"<<endl;
		return 0;
	}

	char cmdline[1024];
	app_manager *app=new app_manager();
	struct BE_env *be_env=(struct BE_env *)malloc(sizeof(struct BE_env));
	//init be_env
	char app_dir[200];
	strcpy(app_dir,argv[1]);
	strcat(app_dir,"/app_data/");
	be_env->app_work_dir=app_dir;
	//printf("%s\n",be_env->app_work_dir);
	be_env->app=app;

	int app_argn;
	void **app_argv=(void **)malloc(sizeof(void *)*(MAX_APP_ARGN+1));
	//init_argv
	app_argv[0]=(void *)be_env;
	//init app_manager
	app->add_app(&help_record);
	app->add_app(&bye_record);
	app->add_app(&graphviz_record);
	app->add_app(&split_tran_record);
	while(1){
		cout<<"%";
		cin.getline(cmdline,1023);
		struct app_record *cur_app;
		cur_app=app->find_app(cmdline,&app_argn,app_argv);
		if(cur_app==NULL){
			cout<<"Can not find this command"<<endl;
			continue;
		}
		int ret;
		ret=run_app(cur_app,app_argn,app_argv);
		release_argv(app_argn,app_argv);
		if(ret==EXIT_BE) break;
	}
}