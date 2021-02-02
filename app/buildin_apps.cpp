#include <iostream>
#include <ostream>
#include <stdio.h>
#include "../include/core_type.h"
#include "../include/app_manager.h"
#include "../include/be.h"

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
