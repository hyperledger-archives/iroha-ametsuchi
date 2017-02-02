#include <ametsuchi/Greeter.h>

namespace ametsuchi {
std::string Greeter::greet() const {
  return std::string("Hello, ") + subject + "!";
}
}
