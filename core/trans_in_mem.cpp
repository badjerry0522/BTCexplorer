#include "../include/core_type.h"
#include "../include/trans_in_mem.h"
#include<unistd.h>
#include<cstring>
#include<cmath>
#include<unordered_set>
#include"../include/block_file.h"
#include"../include/build_TIM.h"
#include<sys/time.h>
int tran_data_size(struct tran_info *tp){
	return (sizeof(ADDR_SEQ)+sizeof(BTC_VOL)<<tp->long_btc_vol)*(tp->input_num+tp->output_num);
}
ERROR_CODE read_TIM_META(char *des_TIM_META,uint64_t *max_addr,uint64_t *num_trans,uint64_t *av_size){
	ifstream fin_TIM_META(des_TIM_META,ios::in);
	if(!fin_TIM_META.is_open()) return CANNOT_OPEN_FILE;
	int version,block_num;
	fin_TIM_META>>version>>block_num>>*num_trans>>*max_addr>>*av_size;
	fin_TIM_META.close();
	return NO_ERROR;
}
ERROR_CODE trans_in_mem::generate_empty_files(char *dir_name,uint64_t max_addr){
	char temp[200];
	strcpy(temp,dir_name);
	strcat(temp,"/addr_show_times.txt");
	FILE *f1=fopen(temp,"wb+");
	if(f1==NULL) return ERROR_FILE;
	creat_addr_show_times(f1,(uint64_t)max_addr*4);
	fclose(f1);
	return NO_ERROR;
}
//return CANNOT_OPEN_FILE when file not exist
ERROR_CODE trans_in_mem::check_is_addr_show_times_exist(char *dir_name){
	char temp[200];
	strcpy(temp,dir_name);
	strcat(temp,"/addr_show_times.txt");
	FILE *f1=fopen(temp,"rb");
	if(f1==NULL){
		cout<<"NULL"<<endl;
		return CANNOT_OPEN_FILE;
	}
	fclose(f1);
	return NO_ERROR;
}
ERROR_CODE generate_av(char *dir_name,char *output_dir_name){
	ERROR_CODE er=NO_ERROR;
    float time_use=0;
    struct timeval start,end;
    gettimeofday(&start,NULL);
    build_TIM(dir_name,0,"",&er);
    gettimeofday(&end,NULL);
    time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);
    cout<<"time_use="<<time_use*1000<<"s"<<endl;
	return er;
}

ERROR_CODE trans_in_mem::init_addr_show_times(char *dir_name){
	ERROR_CODE err;
	//read from TIM_META
	strcpy(des_TIM_meta,dir_name);strcat(des_TIM_meta,"/TIM_meta.txt");
	err=read_TIM_META(des_TIM_meta,&max_addr,&num_trans,&av_size);
	cout<<"max_addr="<<max_addr<<endl;


	//err=generate_empty_files(dir_name,max_addr);//creat empty files
	//cout<<"err of generate_empty_files="<<err<<endl;


	this->dir_name=dir_name;
	strcpy(des_av,dir_name),strcpy(des_TIM_meta,dir_name),
	strcpy(des_trans,dir_name),strcpy(des_addr_show_times,dir_name);//strcpy(des_addr_is_count,dir_name);
	strcat(des_av,"/av.txt"),strcat (des_TIM_meta,"/TIM_meta.txt"),strcat(des_trans,"/trans.txt"),
	strcat(des_addr_show_times,"/addr_show_times.txt");//strcat(des_addr_is_count,"/addr_is_count.txt");
	
	cout<<"des_trans="<<des_trans<<endl;
	cout<<"length of des_trans="<<strlen(des_trans)<<endl;
	cout<<"des_av="<<des_av<<endl;
//getchar();


	//init cache of trans
	err=ca_trans.init(des_trans,31,4,num_trans);
	cout<<"err of init ca_trans="<<err<<endl;
	if(err==ERROR_FILE) return ERROR_FILE;

	err=ca_av.init(des_av,31,2,av_size/4);
	cout<<"err of init ca_av="<<err<<endl;
	if(err==ERROR_FILE) return ERROR_FILE;

	//err=ca_addr_is_count.init(des_addr_is_count,31,2,max_addr);
	//cout<<"err of init ca_addr_is_count="<<err<<endl;

	err=ca_addr_show_times.init(des_addr_show_times,32,2,max_addr);
	cout<<"err of init ca_show_times="<<err<<endl;
	if(err==ERROR_FILE) return ERROR_FILE;

	cout<<"cache init complete"<<endl;
	cout<<endl<<endl;
	//read av to cache
//getchar();
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
		err=ca_trans.load(i,(unsigned char *)&ti);
		if(err!=NO_ERROR) return err;
		int num_addr_in_trans=ti.input_num+ti.output_num;
		//cout<<"i="<<i<<"  num_addr_in_trans="<<num_addr_in_trans<<"  ti.index="<<ti.index<<"  "<<"ti.input_num="<<ti.input_num<<"  ti.output_num="<<ti.output_num<<endl;
		for(int j=0;j<num_addr_in_trans;j++){
			//cout<<"j="<<j<<"   ";
			uint32_t temp[2];
			temp[0]=0;temp[1]=0;
			//read from av
			if(ti.long_btc_vol==1){
				err=ca_av.load(ti.index+j*2,(unsigned char *)&temp[0]);
				if(err!=NO_ERROR) return err;

				err=ca_av.load(ti.index+j*2+1,(unsigned char *)&temp[1]);
				if(err!=NO_ERROR) return err;
				//cout<<"temp[0]="<<temp[0]<<" temp[1]="<<temp[1]<<endl;
			}
			else{
				err=ca_av.load(ti.index+j,(unsigned char *)&temp[1]);	
				if(err!=NO_ERROR) return err;
			}
			cout<<"temp[1]="<<temp[1]<<endl;
			//cout<<"read from av complete"<<endl;
			//write to times
			uint32_t old=0;
			ca_addr_show_times.load(temp[1],(unsigned char *)&old);
			if(err!=NO_ERROR) return err;
			
			//cout<<"load complete"<<endl;
			if(ust.find(temp[1])==ust.end()){
				old++;
				ust.insert(temp[1]);
			}
			//else cout<<"already exist"<<endl;
			//cout<<"old="<<old<<endl;
			err=ca_addr_show_times.store(temp[1],(unsigned char *)&old);
			cout<<"err of store ="<<err<<endl;
			if(err!=NO_ERROR) return err;
			cout<<"store end"<<endl;
		}
		//cout<<"ti.index="<<temp<<endl;
	}
}
trans_in_mem::trans_in_mem(char dir_name[200]){
	ERROR_CODE err;
	err=init_addr_show_times(dir_name);//addr_show_times dont exist
	//
}

int trans_in_mem::tran_num(ADDR_SEQ seq,ERROR_CODE *err){


	

	
	

	
}