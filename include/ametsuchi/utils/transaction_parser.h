//
// Created by hdalpha on 25.05.17.
//

#ifndef AMETSUCHI_TRANSACTION_PARSER_H
#define AMETSUCHI_TRANSACTION_PARSER_H

namespace ametsuchi{
namespace utils{

class TransactionParser {
 public:
  TransactionParser(string raw);
  size_t get_id();
  std::vector<std::string > get_actions();

};

}

}

#endif //AMETSUCHI_TRANSACTION_PARSER_H
