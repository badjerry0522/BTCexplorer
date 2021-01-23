#include <iostream>
#include <ostream>

#include "core_type.h"
#include "trans_in_mem.h"
#include "account.h"
#include "account_set.h"
#include "address_query.h"
#include "address_set.h"
#include "tran_set.h"
using namespace std;
int main(){
	cout<<sizeof(struct block_info)<<" "<<sizeof(struct tran_info)<<" "<<endl;
}