#include "../include/core_type.h"
#include "../include/trans_in_mem.h"
#include<unistd.h>
#include<cstring>
#include<cmath>
#include<unordered_set>
int tran_data_size(struct tran_info *tp){
	return (sizeof(ADDR_SEQ)+sizeof(BTC_VOL)<<tp->long_btc_vol)*(tp->input_num+tp->output_num);
}
ERROR_CODE read_TIM_META(char *des_TIM_META,uint64_t *max_addr,uint64_t *num_trans,uint64_t *av_size){
	ifstream fin_TIM_META(des_TIM_META,ios::in);
	if(!fin_TIM_META.is_open()) return CANNOT_OPEN_FILE;
	int version,block_num;
	fin_TIM_META>>version>>block_num>>*num_trans>>*max_addr>>*av_size;
	return NO_ERROR;
}
trans_in_mem::trans_in_mem(char dir_name[200]){

	this->dir_name=dir_name;
	strcpy(des_av,dir_name),strcpy(des_TIM_meta,dir_name),
	strcpy(des_trans,dir_name),strcpy(des_addr_show_times,dir_name);//strcpy(des_addr_is_count,dir_name);
	strcat(des_av,"/av.txt"),strcat (des_TIM_meta,"/TIM_meta.txt"),strcat(des_trans,"/trans.txt"),
	strcat(des_addr_show_times,"/addr_show_times.txt");//strcat(des_addr_is_count,"/addr_is_count.txt");
	
	cout<<"des_trans="<<des_trans<<endl;
	cout<<"length of des_trans="<<strlen(des_trans)<<endl;
	cout<<"des_av="<<des_av<<endl;
//getchar();

	ERROR_CODE err;
	//read from TIM_META
	err=read_TIM_META(des_TIM_meta,&max_addr,&num_trans,&av_size);
	cout<<"max_addr="<<max_addr<<endl;

	//init cache of trans
	err=ca_trans.init(des_trans,31,4,num_trans);
	cout<<"err of init ca_trans="<<err<<endl;

	err=ca_av.init(des_av,31,2,av_size/4);
	cout<<"err of init ca_av="<<err<<endl;

	//err=ca_addr_is_count.init(des_addr_is_count,31,2,max_addr);
	//cout<<"err of init ca_addr_is_count="<<err<<endl;

	err=ca_addr_show_times.init(des_addr_show_times,32,2,max_addr);
	cout<<"err of init ca_show_times="<<err<<endl;

	cout<<"cache init complete"<<endl;
	cout<<endl<<endl;
	//read av to cache
	
	for(int i=0;i<num_trans;i++){
		//cout<<"num_trans="<<num_trans<<endl;
		unordered_set <uint32_t> ust;
		if(i==num_trans/10) cout<<"10% complete"<<endl;
		if(i==num_trans/5) cout<<"20% complete"<<endl;
		if(i==(int)num_trans/2) cout<<"50% complete"<<endl;
		if(i==(int)(num_trans*0.75)) cout<<"75%complete"<<endl;
		if(i>=(int)(num_trans*0.99)) cout<<"i="<<i<<endl;
		//cout<<"i="<<i<<endl;
		struct tran_info ti;
		ca_trans.load(i,(unsigned char *)&ti);
		int num_addr_in_trans=ti.input_num+ti.output_num;
		//cout<<"i="<<i<<"  num_addr_in_trans="<<num_addr_in_trans<<"  ti.index="<<ti.index<<"  "<<"ti.input_num="<<ti.input_num<<"  ti.output_num="<<ti.output_num<<endl;
		for(int j=0;j<num_addr_in_trans;j++){
			//cout<<"j="<<j<<"   ";
			uint32_t temp[2];
			temp[0]=0;temp[1]=0;
			//read from av
			if(ti.long_btc_vol==1){
				ca_av.load(ti.index+j*2,(unsigned char *)&temp[0]);
				ca_av.load(ti.index+j*2+1,(unsigned char *)&temp[1]);
				//cout<<"temp[0]="<<temp[0]<<" temp[1]="<<temp[1]<<endl;
			}
			else{
				ca_av.load(ti.index+j,(unsigned char *)&temp[1]);
			}
			//cout<<"temp[1]="<<temp[1]<<endl;
			//cout<<"read from av complete"<<endl;
			//write to times
			uint32_t old=0;
			ca_addr_show_times.load(temp[1],(unsigned char *)&old);
			//cout<<"load complete"<<endl;
			if(ust.find(temp[1])==ust.end()){
				old++;
				ust.insert(temp[1]);
			}
			//else cout<<"already exist"<<endl;
			//cout<<"old="<<old<<endl;
			ca_addr_show_times.store(temp[1],(unsigned char *)&old);
			//cout<<"store end"<<endl;
		}
		//cout<<"ti.index="<<temp<<endl;
	}
	//
}

int trans_in_mem::tran_num(ADDR_SEQ seq,ERROR_CODE *err){


	

	
	

	
}