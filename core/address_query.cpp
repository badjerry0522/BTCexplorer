#include "../include/address_query.h"
#include "../include/core_type.h"
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
#include <string.h>
#include <regex.h>
// #include <mysql/mysql.h>

void deleteAllMark(string &s, const string &mark){
    size_t index = 0;
    if(!s.empty()){
        while((index = s.find(mark, index)) != string::npos){
            s.erase(index, 1);
        }
    }
}

bool readDatabaseconfig(char *config, string strconfig[5]){
    ifstream infile; 
    infile.open(config);
    if (!infile.is_open()){
        return false;
    }

    string line;
    int32_t datanum = 0;
    while(getline(infile,line))
    {
        string key;
        string value;
        deleteAllMark(line, " ");
        deleteAllMark(line, "\n");
        deleteAllMark(line, "\r");
        istringstream readstr(line);
        getline(readstr, key, '=');
        getline(readstr, value, '=');
        // cout<<key<<" "<<value<<endl;

        //host, user, password, database, port
        //key=value
        if (key == "host"){
            strconfig[0] = value;
            datanum++;
        }
        else if (key == "user"){
            strconfig[1] = value;
            datanum++;
        }
        else if (key == "password"){
            strconfig[2] = value;
            datanum++;
        }
        else if (key == "database"){
            strconfig[3] = value;
            datanum++;
        }
        else if (key == "port"){
            strconfig[4] = value;
            datanum++;
        }
    }

    infile.close();
    if (datanum != 5)
    {
        return false;
    }
    return true;
}

//enum ADDR_TYPE{P2PKH,P2PSH,SegWit};//types of btc address
ADDR_TYPE getADDR_TYPE(char *address){

    regex_t reP2PKH;
    const char *pattern1 = "^1[a-zA-Z0-9]*$";
    regcomp(&reP2PKH, pattern1, REG_EXTENDED);

    regex_t reP2PSH;
    const char *pattern2 = "^3[a-zA-Z0-9]*$";
    regcomp(&reP2PSH, pattern2, REG_EXTENDED);

    regex_t reSegWit;
    const char *pattern3 = "^bc1[a-zA-Z0-9]*$";
    regcomp(&reSegWit, pattern3, REG_EXTENDED);

    const size_t nmatch = 1;
    regmatch_t pmatch[1];

    ADDR_TYPE type = ADDR_TYPE(-1);

    if (regexec(&reP2PKH, address, nmatch, pmatch, 0) == 0)
    {
        type = P2PKH;
    }
    else if (regexec(&reP2PSH, address, nmatch, pmatch, 0) == 0)
    {
        type = P2PSH;
    }
    else if (regexec(&reSegWit, address, nmatch, pmatch, 0) == 0)
    {
        type = SegWit;
    }
    regfree(&reP2PKH);
    regfree(&reP2PSH);
    regfree(&reSegWit);
    return type;
}

//Init with database name
address_query::address_query(char *dir){
    strcpy(dirname,dir);
    strcpy(databaseconfig,dir);
    strcat(databaseconfig,"databaseconfig");

    strcpy(address_info_fname,dir);
    strcat(address_info_fname,"address_info2.dat");

    strcpy(addr2account_fname,dir);
    strcat(addr2account_fname,"addr2account.dat");
}

//btc_addr --> addr_seq
ADDR_SEQ address_query::get_addr_seq(char* btc_addr,ERROR_CODE *err){

    char BTC2seqdir[MAX_FNAME_SIZE];
    strcpy(BTC2seqdir,dirname);
    strcat(BTC2seqdir,"BTC2seq/");
    return search_btc(BTC2seqdir,btc_addr,err);
/*
    string strconfig[5];
    if (!readDatabaseconfig(databaseconfig, strconfig)){
        *err = CANNOT_OPEN_FILE;
        return NULL_SEQ;
    }

    MYSQL mysql = MYSQL();
    MYSQL_ROW row;
    MYSQL_RES *result;
    // MYSQL_FIELD *field;

    // if(!mysql_real_connect(&mysql, strconfig[0].c_str(), strconfig[1].c_str(), strconfig[2].c_str(), strconfig[3].c_str(), atoi(strconfig[4].c_str()), NULL, 0 ))
    // {
    //     *err = ERROR_FILE;
    //     mysql_close(&mysql);
    //     return NULL_SEQ;
    // }
    if(!mysql_real_connect(&mysql, strconfig[0].c_str(), strconfig[1].c_str(), strconfig[2].c_str(), "bcaddress", atoi(strconfig[4].c_str()), NULL, 0 ))
    {
        *err = ERROR_FILE;
        mysql_close(&mysql);
        return NULL_SEQ;
    }

    char sql[MAX_FNAME_SIZE];
    strcpy(sql,"select id from addr_id where address = '");
    strcat(sql,btc_addr.c_str());
    strcat(sql,"'");

    mysql_query(&mysql, sql);
    result = mysql_store_result(&mysql);
    if(!result){
        *err = INVALID_BTC_ADDR;
        mysql_close(&mysql);
        return NULL_SEQ;
    }

    ADDR_SEQ btc_seq;
    // int num = mysql_num_fields(result);
    row = mysql_fetch_row(result);
    btc_seq = atoi((const char*)row[0]);
    mysql_close(&mysql);

    *err = NO_ERROR;
    return btc_seq;
*/
}

//addr_seq --> btc_addr
ERROR_CODE address_query::get_btc_address(ADDR_SEQ seq,char *btc_addr){
    char seq2BTCdir[MAX_FNAME_SIZE];
    strcpy(seq2BTCdir,dirname);
    strcat(seq2BTCdir,"seq2BTC/");
    return search_addr_seq(seq,seq2BTCdir,btc_addr);
/*
    string strconfig[5];
    if (!readDatabaseconfig(databaseconfig, strconfig)){
        return CANNOT_OPEN_FILE;
    }

    MYSQL mysql = MYSQL();
    MYSQL_ROW row;
    MYSQL_RES *result;
    // MYSQL_FIELD *field;

    // if(!mysql_real_connect(&mysql, strconfig[0].c_str(), strconfig[1].c_str(), strconfig[2].c_str(), strconfig[3].c_str(), atoi(strconfig[4].c_str()), NULL, 0 ))
    // {
    //     mysql_close(&mysql);
    //     return ERROR_FILE;
    // }
    if(!mysql_real_connect(&mysql, strconfig[0].c_str(), strconfig[1].c_str(), strconfig[2].c_str(), "bcaddress", atoi(strconfig[4].c_str()), NULL, 0 ))
    {
        mysql_close(&mysql);
        return ERROR_FILE;
    }

    char sql[MAX_FNAME_SIZE];
    strcpy(sql,"select address from addr_id where id = ");
    ostringstream seqstream;
    seqstream << (uint32_t)seq; 
    string seqstr = seqstream.str();
    strcat(sql,seqstr.c_str());

    mysql_query(&mysql, sql);
    result = mysql_store_result(&mysql);
    if(!result){
        mysql_close(&mysql);
        return INVALID_ADDR_SEQ;
    }

    // int num = mysql_num_fields(result);
    row = mysql_fetch_row(result);
    strcpy(btc_addr, (const char*)row[0]);
    mysql_close(&mysql);

    return NO_ERROR;
*/
}

//fill info with config
void address_query::get_address_info(ADDR_SEQ seq,struct address_info *ai,int config,ERROR_CODE *err){
    //address info file
    ifstream file(address_info_fname, ios::binary);
    if (!file) {
        cout << "can not open file" << endl;
        return;
    }
    else {
        //get size of file
        file.seekg(0, ios::end);
        int file_size = file.tellg();
        int size = file_size / sizeof(address_info2);

        //if out of memory
        if (seq >= size) {
            cout << "INVALID ADDR_SEQ" << endl;
            return;
        }

        //get address info2
        address_info2 info2 = address_info2();
        file.seekg(seq * sizeof(address_info2), ios::beg);
        file.read((char*)&info2, sizeof(address_info2));
        file.close();

        //get account sequence
        ACCOUNT_SEQ acc_seq;
        ifstream accfile(addr2account_fname, ios::binary);
        accfile.seekg(seq * sizeof(ACCOUNT_SEQ), ios::beg);
        accfile.read((char*)&acc_seq, sizeof(ACCOUNT_SEQ));
        accfile.close();

        //get address info
        ai->seq = seq;
        char btc_address[MAX_BTC_ADDRESS_LEN];
        get_btc_address(seq, btc_address);
        strcpy(ai->btc_address, btc_address);
        ai->type = getADDR_TYPE(btc_address);
        ai->info2 = info2;
        ai->account_seq = acc_seq;
        ai->valid = (info2.balance<0?0:1);
    }
	return;
}

//output the addr in os with config
void address_query::output_addr(ADDR_SEQ seq,ostream os,int config){
    //address info file
    ifstream file(address_info_fname, ios::binary);
    if (!file) {
        cout << "can not open file" << endl;
        return;
    }
    else {
        struct address_info *ai;
        //get size of file
        file.seekg(0, ios::end);
        int file_size = file.tellg();
        int size = file_size / sizeof(address_info2);

        //if out of memory
        if (seq >= size) {
            cout << "INVALID ADDR_SEQ" << endl;
            return;
        }

        //get address info2
        address_info2 info2 = address_info2();
        file.seekg(seq * sizeof(address_info2), ios::beg);
        file.read((char*)&info2, sizeof(address_info2));
        file.close();

        //get account sequence
        ACCOUNT_SEQ acc_seq;
        ifstream accfile(addr2account_fname, ios::binary);
        accfile.seekg(seq * sizeof(ACCOUNT_SEQ), ios::beg);
        accfile.read((char*)&acc_seq, sizeof(ACCOUNT_SEQ));
        accfile.close();

        //get address info
        ai->seq = seq;
        char btc_address[MAX_BTC_ADDRESS_LEN];
        get_btc_address(seq, btc_address);
        strcpy(ai->btc_address, btc_address);
        ai->type = getADDR_TYPE(btc_address);
        ai->info2 = info2;
        ai->account_seq = acc_seq;
        ai->valid = (info2.balance<0?0:1);

        switch (config)
        {
        case 0:{
                os<<"seq:"<<ai->seq<<endl;
                break;
            }
        case 1:{
                string address = ai->btc_address;
                os<<"btc_address:"<<address<<endl;
                break;
            }
        case 2:{
                os<<"type:"<<ai->type<<endl;
                break;
            }
        case 3:{
                os<<"address_info2:"<<endl;
                os<<" tran_num:"<<ai->info2.tran_num<<endl;
                os<<" first_block_seq:"<<ai->info2.first_block_seq<<endl;
                os<<" last_block_seq:"<<ai->info2.last_block_seq<<endl;
                os<<" first_clock:"<<ai->info2.first_clock<<endl;
                os<<" last_clock:"<<ai->info2.last_clock<<endl;
                os<<" balance:"<<ai->info2.balance<<endl;
                break;
            }
        case 4:{
                os<<"account_seq:"<<ai->account_seq<<endl;
                break;
            }
        case 5:{
                os<<"valid:"<<ai->valid<<endl;
                break;
            }
            
        //address_info2 
        case 6:{
                os<<"tran_num:"<<ai->info2.tran_num<<endl;
                break;
            }
        case 7:{
                os<<"first_block_seq:"<<ai->info2.first_block_seq<<endl;
                break;
            }
        case 8:{
                os<<"last_block_seq:"<<ai->info2.last_block_seq<<endl;
                break;
            }
        case 9:{
                os<<"first_clock:"<<ai->info2.first_clock<<endl;
                break;
            }
        case 10:{
                os<<"last_clock:"<<ai->info2.last_clock<<endl;
                break;
            }
        case 11:{
                os<<"balance:"<<ai->info2.balance<<endl;
                break;
            }
        default:{
                os<<"seq:"<<ai->seq<<endl;
                string address = ai->btc_address;
                os<<"btc_address:"<<address<<endl;
                os<<"type:"<<ai->type<<endl;
                os<<"address_info2:"<<endl;
                os<<" tran_num:"<<ai->info2.tran_num<<endl;
                os<<" first_block_seq:"<<ai->info2.first_block_seq<<endl;
                os<<" last_block_seq:"<<ai->info2.last_block_seq<<endl;
                os<<" first_clock:"<<ai->info2.first_clock<<endl;
                os<<" last_clock:"<<ai->info2.last_clock<<endl;
                os<<" balance:"<<ai->info2.balance<<endl;
                os<<"account_seq:"<<ai->account_seq<<endl;
                os<<"valid:"<<ai->valid<<endl;
                break;
            }
        }
    }
    return;
}

//output the addr info in the cout with config
void address_query::output_addr(ADDR_SEQ seq,int config){
    //address info file
    ifstream file(address_info_fname, ios::binary);
    if (!file) {
        cout << "can not open file" << endl;
        return;
    }
    else {
        struct address_info *ai;
        //get size of file
        file.seekg(0, ios::end);
        int file_size = file.tellg();
        int size = file_size / sizeof(address_info2);

        //if out of memory
        if (seq >= size) {
            cout << "INVALID ADDR_SEQ" << endl;
            return;
        }

        //get address info2
        address_info2 info2 = address_info2();
        file.seekg(seq * sizeof(address_info2), ios::beg);
        file.read((char*)&info2, sizeof(address_info2));
        file.close();

        //get account sequence
        ACCOUNT_SEQ acc_seq;
        ifstream accfile(addr2account_fname, ios::binary);
        accfile.seekg(seq * sizeof(ACCOUNT_SEQ), ios::beg);
        accfile.read((char*)&acc_seq, sizeof(ACCOUNT_SEQ));
        accfile.close();

        //get address info
        ai->seq = seq;
        char btc_address[MAX_BTC_ADDRESS_LEN];
        get_btc_address(seq, btc_address);
        strcpy(ai->btc_address, btc_address);
        ai->type = getADDR_TYPE(btc_address);
        ai->info2 = info2;
        ai->account_seq = acc_seq;
        ai->valid = (info2.balance<0?0:1);

        switch (config)
        {
        case 0:{
                cout<<"seq:"<<ai->seq<<endl;
                break;
            }
        case 1:{
                string address = ai->btc_address;
                cout<<"btc_address:"<<address<<endl;
                break;
            }
        case 2:{
                cout<<"type:"<<ai->type<<endl;
                break;
            }
        case 3:{
                cout<<"address_info2:"<<endl;
                cout<<" tran_num:"<<ai->info2.tran_num<<endl;
                cout<<" first_block_seq:"<<ai->info2.first_block_seq<<endl;
                cout<<" last_block_seq:"<<ai->info2.last_block_seq<<endl;
                cout<<" first_clock:"<<ai->info2.first_clock<<endl;
                cout<<" last_clock:"<<ai->info2.last_clock<<endl;
                cout<<" balance:"<<ai->info2.balance<<endl;
                break;
            }
        case 4:{
                cout<<"account_seq:"<<ai->account_seq<<endl;
                break;
            }
        case 5:{
                cout<<"valid:"<<ai->valid<<endl;
                break;
            }
        
        //address_info2 
        case 6:{
                cout<<"tran_num:"<<ai->info2.tran_num<<endl;
                break;
            }
        case 7:{
                cout<<"first_block_seq:"<<ai->info2.first_block_seq<<endl;
                break;
            }
        case 8:{
                cout<<"last_block_seq:"<<ai->info2.last_block_seq<<endl;
                break;
            }
        case 9:{
                cout<<"first_clock:"<<ai->info2.first_clock<<endl;
                break;
            }
        case 10:{
                cout<<"last_clock:"<<ai->info2.last_clock<<endl;
                break;
            }
        case 11:{
                cout<<"balance:"<<ai->info2.balance<<endl;
                break;
            }
        default:{
                cout<<"seq:"<<ai->seq<<endl;
                string address = ai->btc_address;
                cout<<"btc_address:"<<address<<endl;
                cout<<"type:"<<ai->type<<endl;
                cout<<"address_info2:"<<endl;
                cout<<" tran_num:"<<ai->info2.tran_num<<endl;
                cout<<" first_block_seq:"<<ai->info2.first_block_seq<<endl;
                cout<<" last_block_seq:"<<ai->info2.last_block_seq<<endl;
                cout<<" first_clock:"<<ai->info2.first_clock<<endl;
                cout<<" last_clock:"<<ai->info2.last_clock<<endl;
                cout<<" balance:"<<ai->info2.balance<<endl;
                cout<<"account_seq:"<<ai->account_seq<<endl;
                cout<<"valid:"<<ai->valid<<endl;
                break;
            }
        }
    }
    
    return;
}