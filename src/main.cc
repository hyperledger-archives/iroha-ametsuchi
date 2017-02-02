#include <iostream>

#include <spdlog/spdlog.h>

#include <ametsuchi/Greeter.h>

int main() {
  ametsuchi::Greeter greeter("Iroha");
  auto log = spdlog::stdout_logger_mt("console");
  log->info("Starting the greeting procedure");
  std::cout << greeter.greet() << std::endl;
  log->info("Finishing the greeting procedure");
  return 0;
}
