#include <iostream>
#include <ostream>
#include <stdio.h>
#include <map>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>

#include "../include/core_type.h"
#include "../include/trans_file.h"
#include "../include/app_manager.h"
#include "../include/be.h"
using namespace std;
#define ADDR2TRAN 1
#define TRAN2ADDR 2
#define ADDR_LEN 6

void begin_gragh(ostream &gout){
	gout<<"digraph finite_state_machine {"<<endl;
	gout<<"rankdir=TB;"<<endl;
//	cout<<"size=\"200,200\""<<endl;
}
void end_graph(ostream &gout){
	gout<<"}"<<endl;
}
void create_tran_node(char *gbuf,struct transaction *t){
	sprintf(gbuf,"t%d[shape=oval label=\"%s\"]",t->seq,t->tran_time);	
}
void create_addr_node(char *gbuf,char *addr1,int seq){
    /*
	string addrstring(addr1);
	map<string, string>::iterator iter;  
	iter = addr_attr_map.find(addrstring);
	char addr[ADDR_LEN+1];
	memcpy(addr,addr1,ADDR_LEN);
	addr[ADDR_LEN]=0;
	if(iter!=addr_attr_map.end()){
		if(iter->second.compare("s")==0){  //Source
			sprintf(gbuf,"a%d[shape=rect color=blue label=\"%s\"]",seq,addr);
		}
		else if(iter->second.compare("d")==0){  //destination
			sprintf(gbuf,"a%d[shape=rect color=green label=\"%s\"]",seq,addr);
		}
		else if(iter->second.compare("entry")==0){  //destination
			sprintf(gbuf,"a%d[shape=rect color=yellow label=\"%s\"]",seq,addr);
		}
		else if(iter->second.compare("exit")==0){  //destination
			sprintf(gbuf,"a%d[shape=rect color=yellowgreen label=\"%s\"]",seq,addr);
		}
		else if(iter->second.compare("o")==0){ //Zero addr
			sprintf(gbuf,"a%d[shape=rect color=red label=\"%s\"]",seq,addr);
		}
		else{ //Unkonwn
			sprintf(gbuf,"a%d[shape=rect color=black label=\"%s\"]",seq,addr);
		}
	}
	else{
		sprintf(gbuf,"a%d[shape=rect  color=black label=\"%s\"]",seq,addr);
	}
    */
   	char addr[ADDR_LEN+1];
	memcpy(addr,addr1,ADDR_LEN);
    sprintf(gbuf,"a%d[shape=rect  color=black label=\"%s\"]",seq,addr);
}
void create_edge(char *gbuf,int a_seq,int t_seq,float bitcoin,int type){
	if(type==ADDR2TRAN){
		sprintf(gbuf,"a%d->t%d[label=\"%f\"]",a_seq,t_seq,bitcoin);
	}
	else if(type==TRAN2ADDR){
		sprintf(gbuf,"t%d->a%d[label=\"%f\"]",t_seq,a_seq,bitcoin);
	}	
}

void add_addrs(ostream &gout,struct addr_tran *ap,int len,map<string,int> *addr_map){
    char gbuf[256];
    string addr_string;
    map<string,int>::iterator iter;
    for(int i=0;i<len;i++){
        addr_string.assign(ap[i].addr);
        iter=addr_map->find(addr_string);
        int seq=addr_map->size();
        if(iter==addr_map->end()){
            create_addr_node(gbuf,ap[i].addr,seq);
            gout<<gbuf<<endl;
            addr_map->insert(pair<string, int>(addr_string,seq));
			seq++;
        }
    }
}

void add_edges(ostream &gout,struct addr_tran *ap,int len,int tseq,map<string,int> *addr_map,int direction){
    char gbuf[256];
    string addr_string;
    map<string,int>::iterator iter;
    for(int i=0;i<len;i++){
        addr_string.assign(ap[i].addr);
        iter=addr_map->find(addr_string);
        create_edge(gbuf,iter->second,tseq,ap[i].bitcoin,direction);
        gout<<gbuf<<endl;
    }
}
void graph_transaction(ostream &gout,struct transaction *t,map<string, int> *addr_map){
	char gbuf[256];
	create_tran_node(gbuf,t);
	gout<<gbuf<<endl;
	
	add_addrs(gout,t->inputs,t->valid_inputs,addr_map);
    add_addrs(gout,t->outputs,t->valid_outputs,addr_map);
    
    add_edges(gout,t->inputs,t->valid_inputs,t->seq,addr_map,ADDR2TRAN);
    add_edges(gout,t->outputs,t->valid_outputs,t->seq,addr_map,TRAN2ADDR);
}
ERROR_CODE graphiviz_tran_app(int app_argn,void **app_argv){
    if(app_argn!=2){
        return INVALID_ARG;
    }
    struct BE_env *env=(struct BE_env *)app_argv[0];
    char *workdir=env->app_work_dir;
    char *trans_fname=(char *)app_argv[1];
    char *png_fname=(char *)app_argv[2];
    struct transaction *p=(struct transaction *)malloc(sizeof(struct transaction));
    trans_file *t=new trans_file();
    ERROR_CODE ret;
    char graph_fname[200];
    map<string, int> addr_map;

    //Init middle file graph.txt in app_word_dir and set to gout
    strcpy(graph_fname,env->app_work_dir);
    strcat(graph_fname,"graph.txt");
    std::filebuf fb;
    //cout<<graph_fname<<endl;
    fb.open (graph_fname,std::ios::out|ios_base::trunc);
    std::ostream gout(&fb);
    
    //Generate graph.txt with the trans_fname;
    begin_gragh(gout);
    ret=t->open_trans_file(trans_fname);
    if(ret!=NO_ERROR)
        return ret;
    t->begin();
    while(1){
        ret=t->next(p);
        if(ret==END_OF_FILE)
            break;
        graph_transaction(gout,p,&addr_map);
    }
    end_graph(gout);

    //Exectue: dot graph.txt -Tpng -o image.png
    char system_line[256];
    strcpy(system_line,"dot ");
    strcat(system_line,graph_fname);
    strcat(system_line," -Tpng -o ");
    strcat(system_line,(char *)app_argv[2]);

    //cout<<system_line<<endl;
    system(system_line);
    //system("dot ./app_data/graph.txt -Tpng -o chain_test.png");

    //Free memory
    free(p);
    fb.close();
    delete t;
    map<string,int>().swap(addr_map);
    malloc_trim(0);
    return NO_ERROR;
}
struct app_record graphviz_record={"graphviz_tran","source_file png_file",graphiviz_tran_app};