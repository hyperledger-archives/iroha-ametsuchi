#pragma once

#include <iostream>
#include <string>

namespace ametsuchi {
class Greeter {
 public:
  Greeter(const char* subject) : subject(subject) {}
  void greet() const;

 private:
  const std::string subject;
};
};
