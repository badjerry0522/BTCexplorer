#include "../include/core_type.h"
#include "../include/trans_in_mem.h"
#include<unistd.h>
#include<cstring>
#include<cmath>
#include <vector>
#include <algorithm>
#include<unordered_set>
#include"../include/block_file.h"
#include"../include/build_TIM.h"
#include<sys/time.h>
int tran_data_size(struct tran_info *tp){
	return (sizeof(ADDR_SEQ)+sizeof(BTC_VOL)<<tp->long_btc_vol)*(tp->input_num+tp->output_num);
}
bool cmp_tran_seq(TRAN_SEQ val,struct block_info &b2){
	return val<b2.first_tran_seq;
}
vector<struct block_info> *read_block(char *fname){
	FILE *fhandle;
	fhandle=fopen(fname,"rb");
//	cout<<fname<<endl;
	if(fhandle==NULL) return NULL;
	vector<struct block_info> *pb=new vector<struct block_info>;
	struct block_info b;
	int bnum=0;
	while(!feof(fhandle)){
		int readnum=fread(&b,sizeof(struct block_info),1,fhandle);
		if(readnum==1)	pb->push_back(b);
//		bnum++;
//		if(bnum%10000==0) cout<<bnum<<endl;
	}
	fclose(fhandle);
	return pb;
}

ERROR_CODE trans_in_mem::read_TIM_META(){
	ifstream fin_TIM_META(TIM_meta_fname,ios::in);
	cout<<TIM_meta_fname<<endl;
	if(!fin_TIM_META.is_open()) return CANNOT_OPEN_FILE;
	int version;
	fin_TIM_META>>version>>block_num>>num_trans>>max_addr>>av_size;
	fin_TIM_META.close();
	return NO_ERROR;
}
trans_in_mem::trans_in_mem(char *_dir_name){
	ERROR_CODE err;
	strcpy(dir_name,_dir_name);
	
	strcpy(av_fname,dir_name);
	strcat(av_fname,"av.dat");

	strcpy(trans_fname,dir_name);
	strcat(trans_fname,"trans.dat");
	
	strcpy(block_fname,dir_name);
	strcat(block_fname,"block.dat");

	strcpy(TIM_meta_fname,dir_name);
	strcat(TIM_meta_fname,"TIM_meta.txt");

	read_TIM_META();
	cout<<"Blocks\t Trans\t Addrs"<<endl;
	cout<<max_block_seq()<<"\t"<<max_tran_seq()<<"\t"<<max_addr_seq()<<endl;
	
	block=read_block(block_fname);
	cout<<"Read "<<block->size()<<" Blocks"<<endl;

	trans_cache=new cache();
	cout<<"Trans Cache:"<<trans_fname<<endl;
	trans_cache->init(trans_fname,30,log2(sizeof(struct tran_info)),(uint64_t)max_tran_seq());

	cout<<"av_cache:"<<av_fname<<endl;
	av_cache=new cache();
	err=av_cache->init(av_fname,31,log2(sizeof(uint32_t)),av_size);
	if(err!=NO_ERROR) cout<<"av_cache init error "<<err<<endl;
	//err=init_addr_show_times(dir_name);//addr_show_times dont exist
	//
}
ADDR_SEQ trans_in_mem::max_addr_seq(){
	return max_addr;
}
TRAN_SEQ trans_in_mem::max_tran_seq(){
	return num_trans;
}
BLOCK_SEQ trans_in_mem::max_block_seq(){
	return block_num;
}

CLOCK trans_in_mem::find_block_time(TRAN_SEQ seq,ERROR_CODE *err){
	vector<struct block_info>::iterator iter;
	if(seq>=num_trans) return *err=INVALID_ADDR_SEQ;
	iter=upper_bound(block->begin(),block->end(),seq,cmp_tran_seq);
	*err=NO_ERROR;
	if(iter!=block->end()){
		iter--;
		return iter->block_time;
	}
	struct block_info last_b=block->at(block->size()-1);
	return last_b.block_time;
}
int trans_in_mem::get_input_num(TRAN_SEQ seq,ERROR_CODE *err){
	if(seq>=num_trans) return INVALID_TRAN_SEQ;
	struct tran_info t;

	ADDR_SEQ temp_addr;
	*err=trans_cache->load(seq,(unsigned char *)&t);
	return t.input_num;
}
int trans_in_mem::get_output_num(TRAN_SEQ seq,ERROR_CODE *err){
	if(seq>=num_trans) return INVALID_TRAN_SEQ;
	struct tran_info t;

	ADDR_SEQ temp_addr;
	*err=trans_cache->load(seq,(unsigned char *)&t);
	return t.output_num;
}
int trans_in_mem::get_input_addr(TRAN_SEQ seq,ADDR_SEQ *addr,ERROR_CODE *err){
	if(seq>=num_trans) return INVALID_TRAN_SEQ;
	struct tran_info t;

	ADDR_SEQ temp_addr;
	*err=trans_cache->load(seq,(unsigned char *)&t);
	uint64_t index=t.index;
	for(int i=0;i<t.input_num;i++){
		*err=av_cache->load(index,(unsigned char *)&temp_addr);
		//cout<<"From Cache"<<addr<<endl;
		addr[i]=temp_addr;
		index++;
	}
	*err=NO_ERROR;
	return t.input_num;
}
int trans_in_mem::get_output_addr(TRAN_SEQ seq,ADDR_SEQ *addr,ERROR_CODE *err){
	if(seq>=num_trans) return INVALID_TRAN_SEQ;
	struct tran_info t;

	ADDR_SEQ temp_addr;
	*err=trans_cache->load(seq,(unsigned char *)&t);
	uint64_t index=t.index+t.input_num;
	for(int i=0;i<t.output_num;i++){
		*err=av_cache->load(index,(unsigned char *)&temp_addr);
		//cout<<"From Cache"<<addr<<endl;
		addr[i]=temp_addr;
		index++;
	}
	*err=NO_ERROR;
	return t.output_num;
}
int trans_in_mem::get_all_addr(TRAN_SEQ seq,ADDR_SEQ *addr,ERROR_CODE *err){
	if(seq>=num_trans) return INVALID_TRAN_SEQ;
	struct tran_info t;

	ADDR_SEQ temp_addr;
	*err=trans_cache->load(seq,(unsigned char *)&t);
	uint64_t index=t.index;
	int sum=t.input_num+t.output_num;
	for(int i=0;i<sum;i++){
		*err=av_cache->load(index,(unsigned char *)&temp_addr);
		//cout<<"From Cache"<<addr<<endl;
		addr[i]=temp_addr;
		index++;
	}
	*err=NO_ERROR;
	return sum;
}

int trans_in_mem::get_input_vol(TRAN_SEQ seq, LONG_BTC_VOL* v, ERROR_CODE *err) {
	if (seq >= num_trans) *err = INVALID_TRAN_SEQ;
	struct tran_info t;
	*err = trans_cache->load(seq, (unsigned char *)&t);


	BTC_VOL vol_short;
	uint64_t index = t.index + t.input_num + t.output_num;

	if (t.long_btc_vol == 0) {
		
		BTC_VOL* temp = new BTC_VOL[t.input_num];
		*err = av_cache->multiload(index,t.input_num, (unsigned char *)temp);
		for (int i = 0; i < t.input_num; i++) {
			v[i] = (LONG_BTC_VOL)temp[i];
		}
		delete temp;
	}
	else {
		
		BTC_VOL* temp = new BTC_VOL[t.input_num*2];
		*err = av_cache->multiload(index,t.input_num*2, (unsigned char *)temp);

		for (int i = 0; i < t.input_num; i++) {
			v[i] = (LONG_BTC_VOL)temp[i*2+1];
			v[i] = (v[i] << 32) + (LONG_BTC_VOL)temp[i*2];
		}
		delete temp;
	}
	*err = NO_ERROR;
	return t.input_num;

}
int trans_in_mem::get_output_vol(TRAN_SEQ seq, LONG_BTC_VOL* v, ERROR_CODE *err) {
	if (seq >= num_trans) *err = INVALID_TRAN_SEQ;
	struct tran_info t;
	*err = trans_cache->load(seq, (unsigned char *)&t);


	BTC_VOL vol_short;
	uint64_t index = t.index + t.input_num + t.output_num;

	if (t.long_btc_vol == 0) {
		index += t.input_num;
		BTC_VOL* temp = new BTC_VOL[t.output_num];
		*err = av_cache->multiload(index, t.output_num, (unsigned char *)temp);
		for (int i = 0; i < t.output_num; i++) {
			v[i] = (LONG_BTC_VOL)temp[i];
		}
		delete temp;
	}
	else {
		index = index + t.input_num * 2;
		BTC_VOL* temp = new BTC_VOL[t.output_num * 2];
		*err = av_cache->multiload(index, t.output_num * 2, (unsigned char *)temp);

		for (int i = 0; i < t.output_num; i++) {
			v[i] = (LONG_BTC_VOL)temp[i*2 + 1];
			v[i] = (v[i] << 32) + (LONG_BTC_VOL)temp[i*2];
		}
		delete temp;
	}
	*err = NO_ERROR;
	return t.output_num;

}


ERROR_CODE trans_in_mem::get_tran_binary(TRAN_SEQ seq,struct transaction_binary *ti){
	if(seq>=num_trans) return INVALID_TRAN_SEQ;
	struct tran_info t;
	ERROR_CODE err;
//	cout<<"get_tran_binary"<<endl;
	err=trans_cache->load(seq,(unsigned char *)&t);
	//printf("From Cache:%d %d %d\n",t.input_num,t.output_num,t.long_btc_vol);

	if(err!=NO_ERROR) return err;
	ti->valid_inputs=t.input_num;
	ti->valid_outputs=t.output_num;
	ti->block_time=find_block_time(seq,&err);
	//cout<<ti->block_time<<endl;

	
	ADDR_SEQ addr;
	uint64_t index=t.index;

	for(int i=0;i<t.input_num;i++){
		err=av_cache->load(index,(unsigned char *)&addr);
		//cout<<"From Cache"<<addr<<endl;
		ti->inputs[i].addr=addr;
		index++;
	}
	for(int i=0;i<t.output_num;i++){
		err=av_cache->load(index,(unsigned char *)&addr);
		ti->outputs[i].addr=addr;
		index++;
	}

//	getchar();
	BTC_VOL vol_short;
	LONG_BTC_VOL vol;
	if(t.long_btc_vol==0){
		for(int i=0;i<t.input_num;i++){	
			err=av_cache->load(index,(unsigned char *)&vol_short);
			ti->inputs[i].bitcoin=(LONG_BTC_VOL)vol_short;
			index++;
		}
		for(int i=0;i<t.output_num;i++){	
			err=av_cache->load(index,(unsigned char *)&vol_short);
			ti->outputs[i].bitcoin=(LONG_BTC_VOL)vol_short;
			index++;
		}
	}
	else{
		for(int i=0;i<t.input_num;i++){
			BTC_VOL vol_low,vol_high;
			err=av_cache->load(index,(unsigned char *)&vol_low);
			index++;
			err=av_cache->load(index,(unsigned char *)&vol_high);
			index++;
			vol=(LONG_BTC_VOL)vol_high;
			vol=(vol<<32)+(LONG_BTC_VOL)vol_low;
	//		cout<<vol_high<<" "<<vol_low<<" "<<vol<<endl;
			ti->inputs[i].bitcoin=vol;
		}
		for(int i=0;i<t.output_num;i++){	
			BTC_VOL vol_low,vol_high;
			err=av_cache->load(index,(unsigned char *)&vol_low);
			index++;
			err=av_cache->load(index,(unsigned char *)&vol_high);
			index++;
			vol=(LONG_BTC_VOL)vol_high;
			vol=(vol<<32)+(LONG_BTC_VOL)vol_low;
	//		cout<<vol_high<<" "<<vol_low<<" "<<vol<<endl;
			ti->outputs[i].bitcoin=vol;
		}
	}
	return NO_ERROR;
}

void trans_in_mem::profile(){
	uint64_t access_num;
	double hit_ratio;
	hit_ratio=av_cache->profile(&access_num);
	show_cache_profile("av cache",access_num,hit_ratio);
	hit_ratio=trans_cache->profile(&access_num);
	show_cache_profile("trans cache",access_num,hit_ratio);
	

}
CLOCK trans_in_mem::first_block_time(){
	struct block_info b=block->at(0);
	return b.block_time;
}
CLOCK trans_in_mem::last_block_time(){
	struct block_info b=block->at(block->size()-1);
	return b.block_time;
}


addr2tran::addr2tran(char *dname){
	char fname[MAX_FNAME_SIZE];
	strcpy(fname,dname);
	strcat(fname,"a2t_meta.txt");
	ifstream fin_meta(fname,ios::in);
	int version;
	ERROR_CODE ret;
	fin_meta>>version>>max_addr>>max_a2t;
	fin_meta.close();
	strcpy(fname,dname);
	strcat(fname,"addr2tran.dat");
	cout<<"a2t cache:\t"<<fname<<endl;
	a2t_cache=new cache();
	ret=a2t_cache->init(fname,30,log2(sizeof(TRAN_SEQ)),max_a2t);
	if(ret!=NO_ERROR){
		cout<<"Error for init cache"<<endl;
	}
	strcpy(fname,dname);
	strcat(fname,"addr2tran_index.dat");
	cout<<"index cache:\t"<<fname<<endl;
	index_cache=new cache();
	ret=index_cache->init(fname,30,log2(sizeof(uint64_t)),max_addr);
	if(ret!=NO_ERROR){
		cout<<"Error for init cache"<<endl;
	}
}
int addr2tran::tran_num(ADDR_SEQ seq, ERROR_CODE *err){
	if(seq>max_addr){
		*err=INVALID_ADDR_SEQ;
		return -1;
	}
	uint64_t index0,index1;
	if(seq==0){
		index0=0;
	}
	else{
		*err=index_cache->load(seq-1,(unsigned char *)&index0);
	}
	*err=index_cache->load(seq,(unsigned char *)&index1);
	*err=NO_ERROR;
	return index1-index0;
}

tran_vec* addr2tran::get_tran_set(ADDR_SEQ seq,ERROR_CODE *err){
	//cout<<"get_tran_set seq="<<seq<<" max_addr="<<max_addr<<endl;
	if(seq>max_addr){
		*err=INVALID_ADDR_SEQ;
		return NULL;
	}
	uint64_t index0,index1;
	if(seq==0){
		index0=0;
	}
	else{
		//cout<<"get_tran_set load index0 seq="<<seq<<endl;
		*err=index_cache->load(seq-1,(unsigned char *)&index0);
		if(*err!=NO_ERROR){
			return NULL;
		}
	}
	*err=index_cache->load(seq,(unsigned char *)&index1);
	if(*err!=NO_ERROR) return NULL;
	//cout<<"get_tran_set index0="<<index0<<" index1="<<index1<<endl;
	tran_vec *tv=new tran_vec();
	TRAN_SEQ tran;
	for(;index0<index1;index0++){
		*err=a2t_cache->load(index0,(unsigned char *)&tran);
		tv->push_back(tran);
	}
	*err=NO_ERROR;
	return tv;
}

void addr2tran::profile(){
	uint64_t access_num;
	double hit_ratio;
	hit_ratio=index_cache->profile(&access_num);
	show_cache_profile("index cache",access_num,hit_ratio);
	hit_ratio=a2t_cache->profile(&access_num);
	show_cache_profile("a2t cache",access_num,hit_ratio);
	
}

