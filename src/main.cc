#include <iostream>

#include <ametsuchi/Greeter.h>

int main() {
  ametsuchi::Greeter greeter("Iroha");
  greeter.greet();
  return 0;
}
