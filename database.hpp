#include <iostream>
#include <map>
#include <fstream>
#include <vector>
#include <cassert>
#include <filesystem>

#ifndef DATABASE_HPP
#define DATABASE_HPP

std::string DB_FILE_NAME = "database.dat"; 
std::string LOG_FILE_NAME = "logfile.dat";
std::string TMP_FILE_NAME = "tmpfile.dat";
std::map<std::string,std::string> map_data;
std::string read_value = "";
std::vector<std::map<std::string,std::string> > queries;
bool isTransact = false;

// base 
namespace mydb {
    std::string helloDB();
    int loadDB();
    int writeDB();

    // 一時ファイルの保存など
    int writeTmpFile();
    int renameTmpfileToWritefile(const std::string& tmpfile, const std::string& writefile);
}

inline std::string mydb::helloDB()
{
    return "Hello My Database!\n";
}

inline int mydb::loadDB()
{
    std::ifstream read_file(DB_FILE_NAME, std::ios::in);
    if (!read_file){
        std::cout << "Error File Can't Open" << std::endl;
        return -1;
    }
    std::string read_line_buffer;
    while (!read_file.eof()){
        std::getline(read_file, read_line_buffer);
        int sep_index = read_line_buffer.find(" ");
        std::string read_key = read_line_buffer.substr(0,sep_index);
        std::string read_value = read_line_buffer.substr(sep_index);
        // TODO 入力の仕様をチェックする
        // std::cout << read_key << ' ' << read_value << std::endl;
        map_data[read_key] = read_value;
    }
    read_file.close();
    return 0;
}

inline int mydb::writeDB()
{
    auto iter = map_data.begin();
    std::ofstream writing_file;
    writing_file.open(DB_FILE_NAME, std::ios::out);
    while(iter != map_data.end()){
        writing_file << iter->first << ' ' << iter->second << std::endl;
        // std::cout << "write completed" << std::endl;
        ++iter;
    }
    writing_file.close();
    return 0;
}

inline int mydb::writeTmpFile()
{   
    std::ofstream writing_tmpfile;
    writing_tmpfile.open(TMP_FILE_NAME, std::ios::out);
    for(const auto &item : map_data){
        writing_tmpfile << item.first << ' ' << item.second << std::endl;
    }
    writing_tmpfile.close();
    return 0;
}

inline int mydb::renameTmpfileToWritefile(const std::string& tmpfile, const std::string& writefile)
{
    std::ofstream[tmpfile];
    std::filesystem::rename(tmpfile,writefile);
}

// memory operation
namespace operate_in_memory {
    // READ
    std::string operateFetch(const std::string& key);

    // WRITE
    int operateUpsert(const std::string& key, const std::string& value);
    int operateDelete(const std::string& key);
}

inline std::string operate_in_memory::operateFetch(const std::string& key)
{
    std::map<std::string, std::string>::iterator itr = map_data.find(key);
    if (itr != map_data.end()) {
        // std::cout << map_data[key] << std::endl;
        read_value = map_data[key];
        return 0;
    } else {
        // std::cout << "key:"<< key << " not found" << std::endl;
        return -1;
    }
}
inline int operate_in_memory::operateUpsert(const std::string& key, const std::string& value = "")
{
    if(value.empty()){
        std::cout << "Value Not Found";
        return -1;
    }
    // map_data[key] = value;
    return 0;
}
inline int operate_in_memory::operateDelete(const std::string& key)
{
    map_data.erase(key);
    return 0;
}

/*
namespace operate_base {
    std::string& operate(const std::string& operation, const std::string& key, const std::string& value);
}
inline std::string& operate_base::operate(const std::string& operation, const std::string& key, const std::string& value="")
{
    std::string& ret = "";
    if(operation == "FETCH"){
        ret = operate_in_memory::operate_fetch(key);
    }
    else if(operation == "UPSERT"){
        ret = operate_in_memory::operate_upsert(key,value);
    }
    else if(operation == "DELETE"){
        ret = operate_in_memory::operate_delete(key);
    }
    return ret;
}
*/

// log
namespace log{
    int readLog();
    int writeLog(std::string l_w, std::string operation, std::string key, std::string value);
};

// 対応関係 writeは一つだがreadは...
inline int log::readLog()
{
    std::ifstream read_file(LOG_FILE_NAME, std::ios::in);
    if (!read_file){
        std::cout << "Error File Can't Open" << std::endl;
        return -1;
    }
    std::string read_line_buffer;
    while (!read_file.eof()){
        std::getline(read_file, read_line_buffer);
        std::cout << read_line_buffer << std::endl;
    }
    read_file.close();
    return 0;
}

/*
inline int log::writeLog(std::string l_w, std::string operation, std::string key, std::string value)
{
    std::ofstream writing_file;
    writing_file.open(LOG_FILE_NAME, std::ios::app);
    // ないときはファイルが新しく作られるので下はいらなそう
    if (!writing_file){
        writing_file.open(LOG_FILE_NAME, std::ios::out);
        std::cout << "Log File Created" << std::endl;
    }
    writing_file << l_w << ' ' << operation << ' ' << key << ' ' << value << std::endl;
    // std::cout << "write completed" << std::endl;
    writing_file.close();
    return 0;
}
*/
inline int log::writeLog(std::map<std::string,std::string>& input_query)
{
    std::ofstream writing_file;
    writing_file.open(LOG_FILE_NAME, std::ios::app);

    if(input_query['value'] != ""){
        writing_file << input_query['ope'] << ' ' << input_query['key'] << ' ' << input_query['val'] << std::endl;
    }
    else{
        writing_file << input_query['ope'] << ' ' << input_query['key'] << std::endl;
    }
    writing_file.close();
    return 0;
}

// Transaction
namespace transaction{

    // beginとcommit
    int begin();
    int commit();

    // それぞれクエリを実行する
    int exec_common_action();
    // read
    std::string t_select();
    // write
    int t_upsert();
    int t_delete();
};

inline int begin()
{
    isTransact = true;
    return 0;
}

inline int commit()
{
    assert(isTransact);
    mydb::writeTmpFile();

    // crcとかの確認を行う？？
    isTransact = false;
    return 0;
}

inline int exec_common_action(const std::map<std::string,std::string>& input_query)
{
    log::writeLog(input_query)
    return 0;
}

inline std::string t_select(const std::string& key)
{
    std::map<std::string,std::string> input_query = {
        {"ope", "select"},
        {"key", key},
        {"value", ""}
    };
    exec_common_action(input_query);
    std::string read_value = operate_in_memory::operateFetch(query["key"]);
    return read_value;
}

inline std::string t_upsert(const std::string& key, const std::string& value="")
{
    std::map<std::string,std::string> input_query = {
        {"ope", "upsert"},
        {"key", key},
        {"value", value}
    };
    exec_common_action(input_query);
    operate_in_memory::operateUpsert(query["key"], query["value"]);
    return 0;
}

inline std::string t_delete(const std::string& key)
{
    std::map<std::string,std::string> input_query = {
        {"ope", "delete"},
        {"key", key},
        {"value", ""}
    };
    exec_common_action(input_query);
    operate_in_memory::operateDelete(query["key"]);
    return 0;
}

// Checkppointing


#endif // DATABASE_HPP8