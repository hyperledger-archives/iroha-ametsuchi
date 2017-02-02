#pragma once

#include <string>

namespace ametsuchi {
class Greeter {
 public:
  Greeter(const char* subject) : subject(subject) {}
  std::string greet() const;

 private:
  const std::string subject;
};
};
