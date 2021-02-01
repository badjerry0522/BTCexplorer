#include "../include/address_query.h"
#include <mysql/mysql.h>
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>

bool readDatabaseconfig(char *config,string strconfig[5]){
    ifstream infile; 
    infile.open(config);
    if (!infile.is_open()){
        return false;
    }

    string line;
    int datanum = 0;
    while(getline(infile,line))
    {
        string key;
        string value;
        istringstream readstr(line);
        getline(readstr, key, '=');
        getline(readstr, value, '=');

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

//Init with database name
address_query::address_query(char *dir){
    strcpy(databaseconfig,dir);
    strcat(databaseconfig,"databaseconfig");

    strcpy(address_info_fname,dir);
    strcat(address_info_fname,"address_info.dat");
}

//btc_addr --> addr_seq
ADDR_SEQ address_query::get_addr_seq(string btc_addr,ERROR_CODE *err){
    string strconfig[5];
    if (!readDatabaseconfig(databaseconfig, strconfig)){
        return NULL_SEQ;
    }

    MYSQL mysql;
    MYSQL_ROW row;
    MYSQL_RES *result;
    MYSQL_FIELD *field;

    if(!mysql_real_connect(&mysql, strconfig[0].c_str(), strconfig[1].c_str(), strconfig[2].c_str(), strconfig[3].c_str(), strconfig[4].c_str(), NULL, 0 ))
    {
        return NULL_SEQ;
    }

    char sql[MAX_FNAME_SIZE];
    strcpy(sql,"select id from addr_id where address = '");
    strcat(sql,btc_addr.c_str());
    strcat(sql,"';");

    mysql_query(&mysql, sql);
    result = mysql_store_result(&mysql);
    if(!result){
        return NULL_SEQ;
    }

    ADDR_SEQ btc_seq;
    // int num = mysql_num_fields(result);
    row = mysql_fetch_row(result);
    btc_seq = atoi((const char*)row[0]);
    mysql_close(&mysql);

    return btc_seq;
}

//addr_seq --> btc_addr
ERROR_CODE address_query::get_btc_address(ADDR_SEQ seq,char *btc_addr){

    string strconfig[5];
    if (!readDatabaseconfig(databaseconfig, strconfig)){
        return INVALID_ADDR_SEQ;
    }

    MYSQL mysql;
    MYSQL_ROW row;
    MYSQL_RES *result;
    MYSQL_FIELD *field;

    if(!mysql_real_connect(&mysql, strconfig[0].c_str(), strconfig[1].c_str(), strconfig[2].c_str(), strconfig[3].c_str(), strconfig[4].c_str(), NULL, 0 ))
    {
        return INVALID_ADDR_SEQ;
    }

    char sql[MAX_FNAME_SIZE];
    strcpy(sql,"select address from addr_id where id = ");
    ostringstream seqstream;
    seqstream << (uint32_t)seq; 
    string seqstr = seqstream.str();
    strcat(sql,seqstr.c_str());
    strcat(sql,";");

    mysql_query(&mysql, sql);
    result = mysql_store_result(&mysql);
    if(!result){
        return INVALID_ADDR_SEQ;
    }

    // int num = mysql_num_fields(result);
    row = mysql_fetch_row(result);
    strcpy(btc_addr, (const char*)row[0]);
    mysql_close(&mysql);

    return NO_ERROR;
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
        int size = file_size / sizeof(address_info);

        //if out of memory
        if (seq >= size) {
            cout << "INVALID ADDR_SEQ" << endl;
            return;
        }

        //get address info
        file.seekg(seq * sizeof(address_info), ios::beg);
        file.read((char*)ai, sizeof(address_info));
        file.close();
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
        int size = file_size / sizeof(address_info);

        //if out of memory
        if (seq >= size) {
            cout << "INVALID ADDR_SEQ" << endl;
            return;
        }

        //get address info
        file.seekg(seq * sizeof(address_info), ios::beg);
        file.read((char*)ai, sizeof(address_info));
        file.close();

        switch (config)
        {
        case 0:
            os<<"seq:"<<ai->seq<<endl;
            break;
        case 1:
            string address = ai->btc_address;
            os<<"btc_address:"<<address<<endl;
            break;
        case 2:
            os<<"type:"<<ai->type<<endl;
            break;
        case 3:
            os<<"address_info2:"<<endl;
            os<<" tran_num:"<<ai->info2.tran_num<<endl;
            os<<" first_block_seq:"<<ai->info2.first_block_seq<<endl;
            os<<" last_block_seq:"<<ai->info2.last_block_seq<<endl;
            os<<" first_clock:"<<ai->info2.first_clock<<endl;
            os<<" last_clock:"<<ai->info2.last_clock<<endl;
            os<<" balance:"<<ai->info2.balance<<endl;
            break;
        case 4:
            os<<"account_seq:"<<ai->account_seq<<endl;
            break;
        case 5:
            os<<"valid:"<<ai->valid<<endl;
            break;
        
        //address_info2 
        case 6:
            os<<"tran_num:"<<ai->info2.tran_num<<endl;
            break;
        case 7:
            os<<"first_block_seq:"<<ai->info2.first_block_seq<<endl;
            break;
        case 8:
            os<<"last_block_seq:"<<ai->info2.last_block_seq<<endl;
            break;
        case 9:
            os<<"first_clock:"<<ai->info2.first_clock<<endl;
            break;
        case 10:
            os<<"last_clock:"<<ai->info2.last_clock<<endl;
            break;
        case 11:
            os<<"balance:"<<ai->info2.balance<<endl;
            break;
        default:
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
        int size = file_size / sizeof(address_info);

        //if out of memory
        if (seq >= size) {
            cout << "INVALID ADDR_SEQ" << endl;
            return;
        }

        //get address info
        file.seekg(seq * sizeof(address_info), ios::beg);
        file.read((char*)ai, sizeof(address_info));
        file.close();

        switch (config)
        {
        case 0:
            cout<<"seq:"<<ai->seq<<endl;
            break;
        case 1:
            string address = ai->btc_address;
            cout<<"btc_address:"<<address<<endl;
            break;
        case 2:
            cout<<"type:"<<ai->type<<endl;
            break;
        case 3:
            cout<<"address_info2:"<<endl;
            cout<<" tran_num:"<<ai->info2.tran_num<<endl;
            cout<<" first_block_seq:"<<ai->info2.first_block_seq<<endl;
            cout<<" last_block_seq:"<<ai->info2.last_block_seq<<endl;
            cout<<" first_clock:"<<ai->info2.first_clock<<endl;
            cout<<" last_clock:"<<ai->info2.last_clock<<endl;
            cout<<" balance:"<<ai->info2.balance<<endl;
            break;
        case 4:
            cout<<"account_seq:"<<ai->account_seq<<endl;
            break;
        case 5:
            cout<<"valid:"<<ai->valid<<endl;
            break;
        
        //address_info2 
        case 6:
            cout<<"tran_num:"<<ai->info2.tran_num<<endl;
            break;
        case 7:
            cout<<"first_block_seq:"<<ai->info2.first_block_seq<<endl;
            break;
        case 8:
            cout<<"last_block_seq:"<<ai->info2.last_block_seq<<endl;
            break;
        case 9:
            cout<<"first_clock:"<<ai->info2.first_clock<<endl;
            break;
        case 10:
            cout<<"last_clock:"<<ai->info2.last_clock<<endl;
            break;
        case 11:
            cout<<"balance:"<<ai->info2.balance<<endl;
            break;
        default:
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
    return;
}