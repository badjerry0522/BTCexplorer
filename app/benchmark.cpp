#include <iostream>
#include <ostream>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <nmmintrin.h>
#include <emmintrin.h>
#include "../include/core_type.h"
#include "../include/trans_file.h"
#include "../include/app_manager.h"
#include "../include/be.h"
#include "../include/tran_vec.h"
#include "../include/statistics.h"
#include "../include/trans_in_mem.h"

int test_log2(uint64_t d) {
	if (d == 0) return 0;
	int i;
	for (i = 0; i < 64; i++) {
		if (d == 0) break;
		d = d >> 1;
	}

	return i - 1;
}

void test_addr(address_query* addrq)
{
	ERROR_CODE err;
	int num = 0;
	float time_use = 0;
	struct timeval start, end;
	gettimeofday(&start, NULL);
	int lines = 0;
	int filenum = 0;
	char srcfname[MAX_FNAME_SIZE];
	while (1)
	{
		filenum++;
		strcpy(srcfname, "./data/address/seq2BTC/seq_btc_");
		char temp[20];
		sprintf(temp, "%d.csv", filenum);
		strcat(srcfname, temp);
		ifstream fin(srcfname, ios::in);
		if (!fin.is_open()) {
			break;
		}
		char line[256];
		
		while (!fin.eof()) {
			char addr_seq[20];
			char BTCaddr[200];

			//Read addr_seq and BTCaddr
			char *token;
			fin.getline(line, 255);
			if (strlen(line) == 0) continue;
			token = strtok(line, ",");
			strcpy(addr_seq, token);
			token = strtok(NULL, " ");
			strcpy(BTCaddr, token);
			int btclen = strlen(BTCaddr);
			int seqlen = strlen(addr_seq);

			if ((btclen > 0) && (seqlen > 0)) {
				if (BTCaddr[0] == '1'&&BTCaddr[1] == 'd'&&BTCaddr[2] == 'i'&&BTCaddr[3] == 'c'&&BTCaddr[4] == 'e') num++;
			}
			else {
				cout << lines << "Empty Line" << endl;
			}
			//if (lines % 100000000 == 0) cout << lines << " " << addr_seq << " " << BTCaddr << endl;

			lines++;
			if (lines > 9829162) break;
		}
		fin.close();
		if (lines > 9829162) break;
	}
	gettimeofday(&end, NULL);
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec) ;
	ofstream f;
	f.open("benchmark.txt", ios::app);
	f << "number of Satoshi Dice address=" << num << endl;
	f << "time_use=" << time_use / 1000000 << "s" << endl;
	f.close();
}

void test_benchmark(trans_in_mem* tim,int maxseq)
{
	ERROR_CODE err;

	BTC_VOL* out = new BTC_VOL[65536];
	BTC_VOL* in = new BTC_VOL[65536];

	//max output value
	long long max_output_value = 0;
	long long max_fee = 0;
	float time_use = 0;
	int long_btc_vol = 0;
	
	struct timeval start, end;
	gettimeofday(&start, NULL);
	//max output value
	for (int i = 0; i < maxseq; i++)
	{
		
		int output_num = tim->get_output_vol(i, out, &err,long_btc_vol);
		if (err == NO_ERROR)
		{
			if (long_btc_vol == 0) {
				for (int j = 0; j < output_num; j++){
					LONG_BTC_VOL vol = (LONG_BTC_VOL)out[j];
					if (max_output_value < vol) max_output_value = vol;
				}
			}
			else {
				for (int j = 0; j < output_num * 2; j += 2) {
					LONG_BTC_VOL vol = ((LONG_BTC_VOL)out[j+1] << 32) + (LONG_BTC_VOL)out[j];
					if (max_output_value < vol) max_output_value = vol;
				}
			}

		}

	}
	gettimeofday(&end, NULL);
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	ofstream f;
	f.open("benchmark.txt");
	f << "max output value=" << max_output_value << endl;
	f << "time use=" << time_use / 1000000 << "s" << endl << endl;

	//TOAD
	/*gettimeofday(&start, NULL);
	uint32_t log_size = test_log2(max_output_value);
	uint64_t* num = new uint64_t[log_size + 1];
	for (int i = 0; i < log_size + 1; i++) num[i] = 0;
	for (int i = 0; i <= maxseq; i++)
	{
		if (err == NO_ERROR)
		{
			int output_num = tim->get_output_vol(i, out, &err);
			for (int j = 0; j < output_num; j++) num[test_log2(out[j])]++;
		}
	}
	for (int i = 0; i < log_size + 1; i++)
	{
		uint64_t l, r;
		if (i == 0)
		{
			f << "0<=value<2: " << num[i] << endl;
		}
		else
		{
			l = (unsigned long)1 << i;
			r = (unsigned long)1 << (i + 1);
			f << l << "<=value<" << r;
			f << ": " << num[i] << endl;
		}

	}
	gettimeofday(&end, NULL);
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	f << "time_use=" << time_use / 1000000 << "s" << endl << endl;*/

	//fee
	gettimeofday(&start, NULL);
	//max output value

	for (int i = 0; i < maxseq; i++)
	{
		long long in_v = 0;
		long long out_v = 0;
		int input_num = tim->get_input_vol(i, in, &err,long_btc_vol);
		if (err == NO_ERROR)
		{
			if (long_btc_vol == 0){
				for (int j = 0; j < input_num; j++)
				{
					in_v += (LONG_BTC_VOL)in[j];
				}
			}
			else {
				for (int j = 0; j < input_num * 2; j+=2) {
					LONG_BTC_VOL vol = ((LONG_BTC_VOL)in[j + 1] << 32) + (LONG_BTC_VOL)in[j];
					in_v += vol;
				}
			}
		}
		int output_num = tim->get_output_vol(i, out, &err,long_btc_vol);
		if (err == NO_ERROR)
		{
			if (long_btc_vol == 0){
				for (int j = 0; j < output_num; j++)
				{
					out_v += (LONG_BTC_VOL)out[j];
				}
			}
			else {
				for (int j = 0; j < output_num * 2; j+=2) {
					LONG_BTC_VOL vol = ((LONG_BTC_VOL)out[j + 1] << 32) + (LONG_BTC_VOL)out[j];
					out_v += vol;
				}
			}

		}
		long long fee = in_v - out_v;
		if (fee > max_fee) max_fee = fee;

	}
	gettimeofday(&end, NULL);
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	f << "max fee=" << max_fee << endl;
	f << "time use=" << time_use / 1000000 << "s" << endl << endl;

	f.close();
	delete[] in;
	delete[] out;

}
void test_benchmark2(trans_in_mem* tim, int maxseq)
{
	ERROR_CODE err;

	BTC_VOL* out = new BTC_VOL[65536];
	BTC_VOL* in = new BTC_VOL[65536];

	//max output value
	long long max_output_value = 0;
	long long max_fee = 0;
	float time_use = 0;
	int long_btc_vol = 0;

	struct timeval start, end;
	gettimeofday(&start, NULL);
	//max output value
	__m128i a, b, c, d;
	__m128i* p;
	__m128i max = _mm_setzero_si128();
	int64_t *res = (int64_t *)malloc(128);
	int index;
	for (int i = 0; i < maxseq; i++)
	{
		int output_num = tim->get_output_vol(i, out, &err, long_btc_vol);

		p = (__m128i*) out;
		index = 0;
		if (err == NO_ERROR)
		{
			if (long_btc_vol == 0) {
				for (index; index + 1 < output_num; index += 2)
				{
					a = _mm_set_epi64x(out[index], out[index + 1]);
					b = _mm_cmpgt_epi64(a, max);
					c = _mm_and_si128(b, a);
					d = _mm_andnot_si128(b, max);
					max = _mm_or_si128(c, d);
					p++;
				}
				memcpy((void *)res, (void *)&max, 128);
				max_output_value = res[0] > res[1] ? res[0] : res[1];
				for (index; index < output_num; index++)
				{
					long long v = out[index];
					max_output_value = v > max_output_value ? v : max_output_value;
				}
			}
			else {
				for (index; index + 1 < output_num; index += 2)
				{
					int64_t v = out[index*2] + ((int64_t)out[index*2 + 1]<<32);
					int64_t v2 = out[(index+1) * 2] + ((int64_t)out[(index+1) * 2 + 1] << 32);
					a = _mm_set_epi64x(v,v2);
					b = _mm_cmpgt_epi64(a, max);
					c = _mm_and_si128(b, a);
					d = _mm_andnot_si128(b, max);
					max = _mm_or_si128(c, d);
					p++;
				}
				memcpy((void *)res, (void *)&max, 128);
				max_output_value = res[0] > res[1] ? res[0] : res[1];
				for (index; index < output_num; index++)
				{
					long long  v =(long long)out[index*2] + ((long long)out[index*2 + 1] << 32);
					if (max_output_value < v) max_output_value = v;
				}
			}

		}

	}
	gettimeofday(&end, NULL);
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	ofstream f;
	f.open("benchmark.txt");
	f << "max output value=" << max_output_value << endl;
	f << "time use=" << time_use / 1000000 << "s" << endl << endl;

	//fee
	gettimeofday(&start, NULL);
	//max output value
	int64_t *fee = (int64_t *)malloc(128);
	__m128i in_tot, out_tot, tot;
	max = _mm_setzero_si128();
	for (int i = 0; i < maxseq; i++)
	{
		index = 0;
		in_tot = _mm_setzero_si128();
		out_tot = _mm_setzero_si128();
		int input_num = tim->get_input_vol(i, in, &err, long_btc_vol);

		p = (__m128i*) in;
		if (err == NO_ERROR)
		{
			if (long_btc_vol == 0) {
				for (index; index + 1 < input_num; index += 2)
				{
					a = _mm_set_epi64x(in[index], in[index + 1]);
					in_tot = _mm_add_epi64(in_tot, a);
					p++;
				}
				for (index; index < input_num; index++)
				{
					a = _mm_set_epi64x(in[index], (int64_t)0);
					in_tot = _mm_add_epi64(in_tot, a);
				}
			}
			else {
				for (index; index + 1 < input_num; index += 2)
				{
					int64_t v = in[index*2] + ((int64_t)in[index*2 + 1] << 32);
					int64_t v2 = in[(index+1) * 2] + ((int64_t)in[(index+1) * 2 + 1] << 32);
					a = _mm_set_epi64x(v, v2);
					in_tot = _mm_add_epi64(in_tot, a);
					p++;
				}
				for (index; index < input_num; index++)
				{
					int64_t v = in[index*2] + ((int64_t)in[index*2 + 1] << 32);
					a = _mm_set_epi64x(v, (int64_t)0);
					in_tot = _mm_add_epi64(in_tot, a);
				}
			}
		}
		int output_num = tim->get_output_vol(i, out, &err, long_btc_vol);

		p = (__m128i*) out;
		index = 0;
		if (err == NO_ERROR)
		{
			if (long_btc_vol == 0) {
				for (index; index + 1 < output_num; index += 2)
				{
					a = _mm_set_epi64x(out[index], out[index + 1]);
					out_tot = _mm_add_epi64(out_tot, a);
					p++;
				}
				for (index; index < output_num; index++)
				{
					a = _mm_set_epi64x(out[index], (int64_t)0);
					out_tot = _mm_add_epi64(out_tot, a);
				}
			}
			else {
				for (index; index + 1 < output_num; index += 2)
				{
					int64_t v = out[index * 2] + ((int64_t)out[index * 2 + 1] << 32);
					int64_t v2 = out[(index + 1) * 2] + ((int64_t)out[(index + 1) * 2 + 1] << 32);
					a = _mm_set_epi64x(v, v2);
					out_tot = _mm_add_epi64(out_tot, a);
					p++;
				}
				for (index; index < output_num; index++)
				{
					int64_t v = out[index * 2] + ((int64_t)out[index * 2 + 1] << 32);
					a = _mm_set_epi64x(v, (int64_t)0);
					out_tot = _mm_add_epi64(out_tot, a);
				}
			}

		}
		tot = _mm_sub_epi64(in_tot, out_tot);
		memcpy((void *)fee, (void *)&tot, 128);

		//max(a,b) = x & a || ~x & b
		a = _mm_set1_epi64x(fee[0] + fee[1]);
		b = _mm_cmpgt_epi64(a, max);
		c = _mm_and_si128(b, a);
		d = _mm_andnot_si128(b, max);
		max = _mm_or_si128(c, d);

	}
	memcpy((void *)res, (void *)&max, 128);
	max_fee = res[0];
	gettimeofday(&end, NULL);
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	f << "max fee=" << max_fee << endl;
	f << "time use=" << time_use / 1000000 << "s" << endl << endl;

	f.close();
	delete[] in;
	delete[] out;
	free(res);
	free(fee);
}




ERROR_CODE benchmark_app(int app_argn, void **argv) {
	//test_tran_vec();
	//test_CLOCK();
	//test_statistics();
	BE_env *env = (BE_env *)argv[0];
	char *opt = (char *)argv[1];
	char *seq = (char*)argv[2];

	int option = atoi(opt);
	int maxseq = atoi(seq);
	address_query *addrq = env->addrq;
	trans_in_mem *tim = env->tim;
	account_query *accq = env->accountq;
	//addr2tran *a2t = env->a2t;
	//cout<<seq<<endl;
	//test_TIM(seq,addrq,tim);
	//test_addr2tran(btc_addr,addrq,tim,a2t);
	//test_benchmark2(tim);
	/*test_benchmark3(tim);*/
	test_benchmark(tim,maxseq);
	if (option == 1) test_benchmark(tim, maxseq);
	else if (option == 2) test_benchmark2(tim, maxseq);
	else if (option == 3);// test_benchmark3(tim, maxseq);
	else if (option == 4);//test_benchmark4(tim, maxseq);
	else ;
	/*test(addrq, accq);*/

	//test_addr(addrq);
	return NO_ERROR;
}

struct app_record benchmark = { "benchmark","option size",benchmark_app };