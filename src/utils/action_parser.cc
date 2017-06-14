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


#include "action_parser.h"
#include <sstream>

namespace utils {

ActionParser::ActionParser(std::string raw) { split(raw); }
void ActionParser::split(std::string raw) {
  char delimeter = ' ';
  std::stringstream ss(raw);  // Turn the string into a stream.
  std::string tok;

  getline(ss, tok, delimeter);
  table_name = tok;

  getline(ss, tok, delimeter);
  key = tok;

  getline(ss, tok, delimeter);
  value = tok;
}
std::string ActionParser::get_value() { return value; }
std::string ActionParser::get_key() { return key; }
std::string ActionParser::get_table_name() { return table_name; }
}
