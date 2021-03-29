#include <iostream>
#include <ostream>
#include <stdio.h>
#include "../include/core_type.h"
#include "../include/trans_file.h"
#include "../include/app_manager.h"
#include "../include/be.h"
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
void test_benchmark(trans_in_mem *tim)
{
	ERROR_CODE err;
	int maxseq = tim->max_tran_seq();
	LONG_BTC_VOL max_output_value = 0;
	float time_use = 0;
	struct timeval start, end;
	gettimeofday(&start, NULL);
	//max output value
	for (int i = 0; i <= maxseq; i++)
	{
		int n = tim->get_output_num(i, &err);
		if (err == NO_ERROR)
		{
			struct transaction_binary *tp = (struct transaction_binary *)malloc(sizeof(struct transaction_binary));
			tim->get_tran_binary(i, tp);
			for (int j = 0; j < n; j++)
			{
				LONG_BTC_VOL val = tp->outputs[j].bitcoin;
				if (val > max_output_value) max_output_value = val;
			}
			free(tp);
		}
	}
	gettimeofday(&end, NULL);
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	ofstream f;
	f.open("benchmark.txt");
	f << "max output value=" << max_output_value << endl;
	f << "time use=" << time_use * 1000 << "s" << endl << endl;

	//TOAD
	gettimeofday(&start, NULL);
	uint32_t log_size = test_log2(max_output_value);
	uint64_t* num = new uint64_t[log_size + 1];
	for (int i = 0; i < log_size + 1; i++) num[i] = 0;
	for (int i = 0; i <= maxseq; i++)
	{
		int n = tim->get_output_num(i, &err);
		if (err == NO_ERROR)
		{
			struct transaction_binary *tp = (struct transaction_binary *)malloc(sizeof(struct transaction_binary));
			tim->get_tran_binary(i, tp);
			for (int j = 0; j < n; j++) num[test_log2(tp->outputs[j].bitcoin)]++;
			free(tp);

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
	f << "time_use=" << time_use * 1000 << "s" << endl << endl;

	f.close();
}
void test_addr(address_query* addrq)
{
	ERROR_CODE err;
	int seq = 0;
	int num = 0;
	float time_use = 0;
	struct timeval start, end;
	gettimeofday(&start, NULL);
	do
	{
		char btc_addr[256];
		err = addrq->get_btc_address(seq, btc_addr);
		seq++;
		if (btc_addr[0] == '1'&&btc_addr[1] == 'd'&&btc_addr[2] == 'i'&&btc_addr[3] == 'c'&&btc_addr[4] == 'e')  num++;

	} while (err != INVALID_ADDR_SEQ);
	gettimeofday(&end, NULL);
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

	ofstream f;
	f.open("benchmark.txt", ios::app);
	f << "number of Satoshi Dice address=" << num << endl;
	f << "time_use=" << time_use * 1000 << "s" << endl;
	f.close();
}
ERROR_CODE benchmark_app(int app_argn, void **argv) {
	//test_tran_vec();
	//test_CLOCK();
	//test_statistics();
	BE_env *env = (BE_env *)argv[0];
	address_query *addrq = env->addrq;
	trans_in_mem *tim = env->tim;
	//addr2tran *a2t = env->a2t;
	//cout<<seq<<endl;
	//test_TIM(seq,addrq,tim);
	//test_addr2tran(btc_addr,addrq,tim,a2t);
	test_benchmark(tim);
	//test_addr(addrq);
	return NO_ERROR;
}

struct app_record benchmark = { "benchmark","NULL",benchmark_app };