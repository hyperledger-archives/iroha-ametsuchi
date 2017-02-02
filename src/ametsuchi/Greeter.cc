#include <ametsuchi/Greeter.h>

namespace ametsuchi {
void Greeter::greet() const {
  std::cout << "Hello, " << subject << "!" << std::endl;
}
}
