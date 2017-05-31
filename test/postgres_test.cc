/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <pqxx/pqxx>
#include <iostream>

TEST(postgres_test, sample_test) {
  try
  {
    pqxx::connection C;
    std::cout << "Connected to " << C.dbname() << std::endl;
    ASSERT_TRUE(C.is_open());
//    pqxx::work W(C);
//
//    pqxx::result R = W.exec("SELECT name FROM employee");
//
//    std::cout << "Found " << R.size() << "employees:" << std::endl;
//    for (auto row: R)
//      std::cout << row[0].c_str() << std::endl;
//
//    std::cout << "Doubling all employees' salaries..." << std::endl;
//    W.exec("UPDATE employee SET salary=salary*2");
//
//    std::cout << "Making changes definite: ";
//    W.commit();
//    std::cout << "ok." << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
  }
}
