#include <iostream>
#include <cassert>
#include <random>
#include <array>
#include <algorithm>
#include <random>
#include "database.hpp"
// #include "CppUTest/CommandLineTestRunner.h"
// #include "CppUTest/TestHarness.h"

int RANDOM_NUM_OF_TRIALS = 1000;
std::uniform_int_distribution<> rand100(0, 10000);
std::random_device rnd;
std::mt19937 mt(rnd());

std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ mt() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

/*
TEST_GROUP(TEST_First){};


TEST(TEST_First, Test1)
{
  CHECK_EQUAL("Hello My Database!\n",mydb::helloDB());
  CHECK_EQUAL("1","1");
}
*/
/*
void first_test(){
  mydb::loadDB();

  // moery operationのテスト
  operate_base::operate("UPSERT","hogefuga","banana");
  operate_base::operate("READ","hogefuga");
  operate_base::operate("DELETE","hogefuga");
  // errorになるやつ
  operate_base::operate("UPSERT","hogefuga");
  mydb::writeDB();
}
*/

void transaction_test(){
  std::string key1 = "name";
  std::string value1 = "hoge";
  std::string key2 = "name2";
  std::string value2 = "hogehoge";

  transaction::t_upsert(key1,value1);
  transaction::t_upsert(key2,value2);
  assert(2 == map_data.size());
  assert("hoge" == transaction::t_select(key1));
  assert("hogehoge" == transaction::t_select(key2));
  transaction::t_delete(key2);
  assert(1 == map_data.size());
  transaction::t_delete(key1);

  std::cout << "transaction test completed!!" << std::endl;
}

void transaction_test_many(){
  for(int i = 0; i < RANDOM_NUM_OF_TRIALS; ++i){
    std::array<std::string,4> tmp_strings;
    for(int j = 0; j < 4; ++j){
      const int length = rand100(mt);
      tmp_strings[j] = random_string(length);
    }
    std::string key1 = tmp_strings[0];
    std::string value1 = tmp_strings[1];
    std::string key2 = tmp_strings[2];
    std::string value2 = tmp_strings[3];
    transaction::t_upsert(key1,value1);
    transaction::t_upsert(key2,value2);
    assert(i + 2 == map_data.size());
    assert(value2 == transaction::t_select(key2));
    transaction::t_delete(key2);
    assert(i + 1 == map_data.size());
  }
  std::cout << "transaction test many completed!!" << std::endl;
}

int main(int argc, char **argv){
  // const char *fileName = "C:\\test.txt";
  // log::write_log("WRITE","name","hoge");
  // log::write_log("DELETE","name","hoge");
  // log::read_log();

  // first_test();
  // return RUN_ALL_TESTS(argc, argv);
  transaction_test();
  transaction_test_many();
  return 0;
}
