//
// Created by hdalpha on 25.05.17.
//

#ifndef AMETSUCHI_TRANSACTION_PARSER_H
#define AMETSUCHI_TRANSACTION_PARSER_H
#include <string>
#include <vector>

namespace ametsuchi{
namespace utils{

class TransactionParser {
 public:
  TransactionParser(std::string raw);
  std::string get_hash();
  std::vector<std::string > get_actions();
 private:
  std::string hash;
  std::vector<std::string> actions;
  void split(std::string raw);

};

}

}

#endif //AMETSUCHI_TRANSACTION_PARSER_H
