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
    int operateFetch(const std::string& key);

    // WRITE
    int operateUpsert(const std::string& key, const std::string& value);
    int operateDelete(const std::string& key);
}

inline int operate_in_memory::operateFetch(const std::string& key)
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
inline int log::writeLog(const std::vector<string>& input_queries)
{
    std::ofstream writing_file;
    writing_file.open(LOG_FILE_NAME, std::ios::app);
    // ないときはファイルが新しく作られるので下はいらなそう
    /*
    if (!writing_file){
        writing_file.open(LOG_FILE_NAME, std::ios::out);
        std::cout << "Log File Created" << std::endl;
        return -1;
    }
    */
    for(std::string query : queries){
        writing_file << query << std::endl;
    }
    writing_file.close();
    return 0;
}

// Transaction
namespace transaction{
    // クエリの実行
    int execQuery();
    int addQuery(const std::string& operation, const std::string& key, const std::string& value="");
    int classificateOrder(const std::string& operation, const std::string& key, const std::string& value="");

    // beginとcommit
    int begin();
    int commit();
};

inline int addQuery(const std::string& operation, const std::string& key, const std::string& value="")
{
    std::map<std::string,std::string> input_query = {
        {"Ope", operation},
        {"Key", key},
        {"Value", value}
    };
    (operation=="READ") ? q["W/L"] = "READ" : q["W/L"] = "WRITE";
    queries.push_back(input_query);
    return 0;
}

inline int execQuery()
{
    if(queries.size()==0){
        std::cout << "QueryNotFoundError" << std::endl;
        return -1;
    }
    log::writeLog(queries);
    for(std::map<std::string,std::string> query : queries){
        if(query["Ope"] == "READ"){
            operate_in_memory::operateFetch(query["key"]);
        }
        else if(query["Ope"] == "UPSERT"){
            operate_in_memory::operateUpsert(query["key"], query["value"]);
        }
        else if(query["Ope"] == "DELETE"){
            operate_in_memory::operateDelete(query["key"])
        }
        else{
            std::cout << "QueryOpeNotFoundError" << std::endl;
            return -1; 
        }
    }
    // mapの処理が終わったので、readの処理とコミットとかをする
    mydb::writeTmpFile();
}

inline int classificateOrder(const std::string& operation, const std::string& key, const std::string& value="")
{
    if(operation == "FETCH"){

    }
}


// Checkppointing


#endif // DATABASE_HPP8