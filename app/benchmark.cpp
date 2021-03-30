#include <iostream>
#include <ostream>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
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
		}
		fin.close();
	}
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
	test_addr(addrq);
	return NO_ERROR;
}

struct app_record benchmark = { "benchmark","NULL",benchmark_app };