//
// Created by hdalpha on 25.05.17.
//

#ifndef AMETSUCHI_ACTION_PARSER_H
#define AMETSUCHI_ACTION_PARSER_H
#include <string>
#include <vector>

namespace utils {

class ActionParser {
 public:
  ActionParser(std::string raw);
  std::string get_table_name();
  std::string get_key();
  std::string get_value();

 private:
  std::string key;
  std::string value;
  std::string table_name;
  void split(std::string raw);
};
}


#endif  // AMETSUCHI_ACTION_PARSER_H
