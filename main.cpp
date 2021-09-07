#include <iostream>
#include "database.hpp"
#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"

int USER_INPUT_QUERY_NUM = 3;

TEST_GROUP(TEST_First){};


TEST(TEST_First, Test1)
{
  CHECK_EQUAL("Hello My Database!\n",mydb::helloDB());
  
}
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

int main(int ac, char **av){
  // const char *fileName = "C:\\test.txt";
  // log::write_log("WRITE","name","hoge");
  // log::write_log("DELETE","name","hoge");
  // log::read_log();

  // first_test();
  // return 0;
  return RUN_ALL_TESTS(ac, av);
}
