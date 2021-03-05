#include "../include/app_manager.h"
#include "../include/be.h"

extern struct app_record graphviz_record;
extern struct app_record split_tran_record;
extern struct app_record select_tran_record;
extern struct app_record addr_of_tran_record;
extern struct app_record analyse_tran_record;
extern struct app_record account_of_addr_record;
extern struct app_record test_record;
extern struct app_record help_record;
extern struct app_record bye_record;
extern struct app_record profile_record;


void add_app(app_manager *app){
    app->add_app(&help_record);
	app->add_app(&bye_record);
	app->add_app(&graphviz_record);
	app->add_app(&split_tran_record);
	app->add_app(&select_tran_record);
	app->add_app(&addr_of_tran_record);
	app->add_app(&analyse_tran_record);
	app->add_app(&account_of_addr_record);
	app->add_app(&test_record);
	app->add_app(&profile_record);
}