#include <iostream>
#include <ostream>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include "../include/core_type.h"
#include "../include/trans_file.h"
#include "../include/app_manager.h"
#include "../include/be.h"
#include "../include/tran_vec.h"
#include "../include/statistics.h"
#include "../include/trans_in_mem.h"



//convert time stamp
string stamp_to_standard(int stampTime)
{
	time_t tick = (time_t)stampTime;
	struct tm tm;
	char s[30];
	tm = *localtime(&tick);
	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);
	// printf("%d: %s\n", (int)tick, s);
	string timestr = s;
	return timestr;
}

//power of two
bool isPowerOfTwo(int64_t n) {
	return (n > 0) && (n & (n - 1)) == 0;
}

//save data
void savedata(map<int64_t, map<int, int> > dataforpool, string outfile) {
	ofstream outFile3;
	outFile3.open(outfile.data(), ios::out);
	map<int64_t, map<int, int> >::iterator iter;
	for (iter = dataforpool.begin(); iter != dataforpool.end(); iter++) {
		outFile3 << "value:" << ((float)(iter->first)) / 100000000 << endl;

		int sum = 0;
		int num = 0;
		int max = 0;
		int min = 100000;
		map<int, int>::iterator seiter;
		for (seiter = iter->second.begin(); seiter != iter->second.end(); seiter++) {
			sum = sum + (seiter->first * seiter->second);
			num = num + seiter->second;

			max = max > seiter->first ? max : seiter->first;
			min = min < seiter->first ? min : seiter->first;
		}

		outFile3 << "max:" << max << " ";
		outFile3 << "min:" << min << " ";
		outFile3 << "all:" << num << " ";
		outFile3 << "avg:" << ((double)sum / num) << endl;
		outFile3 << endl;
	}
	outFile3.close();
}
void savedata2(map<ADDR_SEQ, int64_t> dataforpool, string outfile) {
	ofstream outFile3;
	outFile3.open(outfile.data(), ios::out);
	map<ADDR_SEQ,int64_t>::iterator iter;
	for (iter = dataforpool.begin(); iter != dataforpool.end(); iter++) {
		outFile3 << iter->first << " " << iter->second;
		outFile3 << endl;
	}
	outFile3.close();
}


void getMixPool(trans_in_mem* tim, address_query* addrq) {
	float time_use = 0;
	struct timeval start, end;
	gettimeofday(&start, NULL);
	ofstream outFile1;
	outFile1.open("chaintx1.txt", ios::out);

	ofstream outFile2;
	outFile2.open("chaintx2.txt", ios::out);

	map<int64_t, map<int, int> > dataforpool1;
	map<int64_t, map<int, int> > dataforpool2;
	map<ADDR_SEQ, int64_t> addresses;


	ERROR_CODE err;
	struct transaction_binary *tp = (struct transaction_binary *)malloc(sizeof(struct transaction_binary));
	TRAN_SEQ max_seq = tim->max_tran_seq();
	
	for (int i = 0; i < max_seq; i++) {

		CLOCK blocktime = tim->find_block_time(i, &err);

		if (blocktime >= 1610239070 || blocktime <= 1577808000) continue;
		
		int output_num = tim->get_output_num(i, &err);


		if (output_num > 1) {
			int ispool = 0;
			set<LONG_BTC_VOL> vset;
			int64_t poolvalue = 0;

			tim->get_tran_binary(i, tp);
			for (unsigned int index = 0; index < output_num; index++) {

				LONG_BTC_VOL outvalue = tp->outputs[index].bitcoin;
				vset.insert(outvalue);

				if (vset.size() > 2) {
					// issave = false;
					vset.clear();
					break;
				}
				if (outvalue != 0 && outvalue % 100000 == 0 && isPowerOfTwo((int64_t)(outvalue / 100000)) != 0) {
					ispool++;
					poolvalue = outvalue;
				}

			}

			if (ispool == output_num || (ispool >= 2 && ispool == (output_num - 1))) {
				
				int input_num = tim->get_input_num(i, &err);
				map<ADDR_SEQ, int64_t>::iterator l_it;

				//I
				if (vset.size() == 1) {

					outFile1 << stamp_to_standard(blocktime) << endl;
					for (unsigned int index = 0; index < input_num; index++) {
						
						char btc_addr[256];
						err = addrq->get_btc_address(tp->inputs[index].addr, btc_addr);
						string addr =btc_addr ;
						outFile1 << addr << " " << ((double)tp->inputs[index].bitcoin / 100000000) << endl;
					}
					outFile1 << "TO" << endl;
					for (unsigned int index = 0; index < output_num; index++) {
					
						char btc_addr[256];
						err = addrq->get_btc_address(tp->outputs[index].addr, btc_addr);
						string addr = btc_addr;
						outFile1 << addr << " " << ((double)tp->outputs[index].bitcoin / 100000000) << endl;

						l_it = addresses.find(tp->outputs[index].addr);
						if(l_it == addresses.end()) addresses.insert(map<ADDR_SEQ, int64_t>::value_type(tp->outputs[index].addr, tp->outputs[index].bitcoin));

					}
					outFile1 << endl;

					// Statistics pool
					map<int64_t, map<int, int> >::iterator iteration;
					iteration = dataforpool1.find(poolvalue);
					int size = output_num;
					if (dataforpool1.size() == 0 || iteration == dataforpool1.end()) {
						map<int, int> temp;
						temp.insert(map<int, int>::value_type(size, 1));
						dataforpool1.insert(map<int64_t, map<int, int> >::value_type(poolvalue, temp));
					}
					else {
						map<int, int>::iterator seiteration;
						seiteration = iteration->second.find(size);
						if (iteration->second.size() == 0 || seiteration == iteration->second.end()) {
							iteration->second.insert(map<int, int>::value_type(size, 1));
						}
						else
						{
							seiteration->second = seiteration->second + 1;
							seiteration = iteration->second.find(-1); //max
							seiteration->second = (seiteration->second > size ? seiteration->second : size);
							seiteration = iteration->second.find(-2); //min
							seiteration->second = (seiteration->second < size ? seiteration->second : size);
						}
					}


				}
				//II
				if (vset.size() == 2) {
					outFile2 << stamp_to_standard(blocktime) << endl;
					for (unsigned int index = 0; index < input_num; index++) {
						
						char btc_addr[256];
						err = addrq->get_btc_address(tp->inputs[index].addr, btc_addr);
						string addr = btc_addr;
						outFile2 << addr << " " << ((double)tp->inputs[index].bitcoin / 100000000) << endl;

					}
					outFile2 << "TO" << endl;
					for (unsigned int index = 0; index < output_num; index++) {
						
						char btc_addr[256];
						err = addrq->get_btc_address(tp->outputs[index].addr, btc_addr);
						string addr = btc_addr;
						outFile2 << addr << " " << ((double)tp->outputs[index].bitcoin / 100000000) << endl;

						l_it = addresses.find(tp->outputs[index].addr);
						if (l_it == addresses.end()) addresses.insert(map<ADDR_SEQ, int64_t>::value_type(tp->outputs[index].addr, tp->outputs[index].bitcoin));
					}
					outFile2 << endl;



						// Statistics pool
						map<int64_t, map<int, int> >::iterator iteration;
						iteration = dataforpool2.find(poolvalue);
						int size = output_num;


						if (dataforpool2.size() == 0 || iteration == dataforpool2.end()) {
							map<int, int> temp;
							temp.insert(map<int, int>::value_type(size, 1));
							dataforpool2.insert(map<int64_t, map<int, int> >::value_type(poolvalue, temp));
						}
						else {
							map<int, int>::iterator seiteration;
							seiteration = iteration->second.find(size);
							if (iteration->second.size() == 0 || seiteration == iteration->second.end()) {
								iteration->second.insert(map<int, int>::value_type(size, 1));
							}
							else
							{
								seiteration->second = seiteration->second + 1;
								seiteration = iteration->second.find(-1); //max
								seiteration->second = (seiteration->second > size ? seiteration->second : size);
								seiteration = iteration->second.find(-2); //min
								seiteration->second = (seiteration->second < size ? seiteration->second : size);
							}
						}
					


				}
				vset.clear();
			}
		}
	}
	
	cout << "Save data ............." << endl;
	savedata(dataforpool1, "chaintxdata1.txt");
	savedata(dataforpool2, "chaintxdata2.txt");
	savedata2(addresses, "chaintxaddress2020.txt");

	gettimeofday(&end, NULL);
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	cout << "get mix pool time use=" << time_use / 1000000 << "s" << endl << endl;


	outFile1.close();
	outFile2.close();
	dataforpool1.clear();
	dataforpool2.clear();
	addresses.clear();
	free(tp);
}


void ChipMix(trans_in_mem* tim,address_query* addrq) {

	float time_use = 0;
	struct timeval start, end;
	gettimeofday(&start, NULL);

	//I chipMix output transaction
	ofstream exitTran1;
	exitTran1.open("ChipMix_2020_exitTran_I.txt", ios::out);

	//II ChipMix output transaction
	ofstream exitTran2;
	exitTran2.open("ChipMix_2020_exitTran_II.txt", ios::out);

	//II ChipMix input address not belong to pool and its output value
	ofstream exitOdd;
	exitOdd.open("ChipMix_2020_exitOdd_addr.txt", ios::out);

	//I and II ChipMix output address and value 
	ofstream dest;
	dest.open("ChipMix_2020_dest_addr.txt", ios::out);

	string csvpath = "chaintxaddress2020.txt";
	map<ADDR_SEQ, int64_t> addresses;//map
	if (!csvpath.empty()) {
		ifstream fp(csvpath.data());
		string line;

		
		while (getline(fp, line)) {

			string idstr;
			string vs;
			istringstream readstr(line); 
			//split
			getline(readstr, idstr, ' '); //id
			getline(readstr, vs, ' '); //value
			int64_t vv = atoll(vs.c_str());
			int id = atoll(idstr.c_str());
			addresses.insert(map<ADDR_SEQ, int64_t>::value_type((ADDR_SEQ)id, vv));
		}
		fp.close();
	}
	cout << "addresses:" << addresses.size() << endl;



	ERROR_CODE err;
	struct transaction_binary *tp = (struct transaction_binary *)malloc(sizeof(struct transaction_binary));
	TRAN_SEQ max_seq = tim->max_tran_seq();
	for (int i = 0; i < max_seq; i++) {

		CLOCK blocktime = tim->find_block_time(i,&err);
		if (blocktime >= 1610239070 || blocktime <= 1577808000) continue;
		int output_num = tim->get_output_num(i,&err);

		if (output_num == 1) {
			int input_num = tim->get_input_num(i,&err);
			int ischip = 0; //belong to pool

			//II ChipMix address not belong to pool and value 
			int64_t value2 = 0;
			string address2 = "";

			bool II = true;
			tim->get_tran_binary(i, tp);
			
			for (int index = 0; index < input_num; index++) {
				LONG_BTC_VOL v = tp->inputs[index].bitcoin;
				char addr[256];
				err = addrq->get_btc_address(tp->inputs[index].addr, addr);
				string address = addr;
				map<ADDR_SEQ, int64_t>::iterator l_it;
				l_it = addresses.find(tp->inputs[index].addr);

				if (l_it != addresses.end() && v == l_it->second)
				{
					ischip++;
					l_it->second = -10;  //tab used
				}
				else
				{
					value2 = v;
					address2 = address;
					if (v != 0 && v % 100000 == 0 && isPowerOfTwo((int64_t)(v / 100000)) != 0)
					{
						II = false;
					}
				}
			}

			//I chipMix
			if (ischip == input_num && ischip > 0)
			{
				//Ê±¼ä
				exitTran1 << stamp_to_standard(blocktime) << endl;
				dest << stamp_to_standard(blocktime) << endl;
				for (unsigned int index = 0; index < input_num; index++) {
					
					char btc_addr[256];
					err = addrq->get_btc_address(tp->inputs[index].addr, btc_addr);
					string addr = btc_addr;
					exitTran1 << addr << " " << ((double)tp->inputs[index].bitcoin / 100000000) << endl;
				}

				exitTran1 << "TO" << endl;

				for (unsigned int index = 0; index < output_num; index++) {
					
					char btc_addr[256];
					err = addrq->get_btc_address(tp->outputs[index].addr, btc_addr);
					string addr = btc_addr;
					exitTran1 << addr << " " << ((double)tp->outputs[index].bitcoin / 100000000) << endl;
					dest << addr << " " << ((double)tp->outputs[index].bitcoin / 100000000) << endl;
				}

				exitTran1 << endl;
				dest << endl;
			}

			//II ChipMix
			if (ischip == (input_num - 1) && ischip > 0 && II == true)
			{
				//time
				exitTran2 << stamp_to_standard(blocktime) << endl;
				dest << stamp_to_standard(blocktime) << endl;
				for (unsigned int index = 0; index < input_num; index++) {
					
					char btc_addr[256];
					err = addrq->get_btc_address(tp->inputs[index].addr, btc_addr);
					string addr = btc_addr;
					exitTran2 << addr << " " << ((double)tp->inputs[index].bitcoin / 100000000) << endl;
				}

				exitTran2 << "TO" << endl;

				for (unsigned int index = 0; index < output_num; index++) {
					
					char btc_addr[256];
					err = addrq->get_btc_address(tp->outputs[index].addr, btc_addr);
					string addr = btc_addr;
					exitTran2 << addr << " " << ((double)tp->outputs[index].bitcoin / 100000000) << endl;
					dest << addr << " " << ((double)tp->outputs[index].bitcoin / 100000000) << endl;

				}

				exitTran2 << endl;
				dest << endl;

				exitOdd << stamp_to_standard(blocktime) << endl;
				exitOdd << address2 << " " << ((double)value2 / 100000000) << endl;
				exitOdd << endl;
			}
		}

		
	}
	exitTran1.close();
	exitTran2.close();
	exitOdd.close();
	dest.close();

	ofstream unused;
	unused.open("ChipMix_2020_unused.txt", ios::out);
	map<ADDR_SEQ, int64_t>::iterator iter;
	for (iter = addresses.begin(); iter != addresses.end(); iter++) {
		if (iter->second != -10)
		{
			char btc_addr[256];
			err = addrq->get_btc_address(iter->first, btc_addr);
			string addr = btc_addr;
			unused << addr << " " << iter->second << endl;
		}
	}
	unused.close();
	addresses.clear();


	gettimeofday(&end, NULL);
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	cout << "chipmix time use=" << time_use / 1000000 << "s" << endl << endl;

	delete tp;
}

ERROR_CODE chipmix_app(int app_argn, void **argv) {

	BE_env *env = (BE_env *)argv[0];
	address_query *addrq = env->addrq;
	trans_in_mem *tim = env->tim;

	
	getMixPool(tim, addrq);
	ChipMix(tim,addrq);


	return NO_ERROR;
}

struct app_record chip_mix_record = {"chip_mix","NULL",chipmix_app };