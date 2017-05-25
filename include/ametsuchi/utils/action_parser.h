//
// Created by hdalpha on 25.05.17.
//

#ifndef AMETSUCHI_ACTION_PARSER_H
#define AMETSUCHI_ACTION_PARSER_H

namespace ametsuchi{
namespace utils{

class ActionParser {
 public:
  ActionParser(string raw);
  std::string get_table_name();
  std::string get_key();
  std::string get_value();
};

}
}

#endif //AMETSUCHI_ACTION_PARSER_H
