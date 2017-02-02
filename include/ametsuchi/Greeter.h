#pragma once

#include <string>

namespace ametsuchi {
/// Greets it's caller.
/// This class tries to determine caller's psycological profile
/// using multiple heuristics.
class Greeter {
 public:
  /// Creates the greeter and calculates psychological profile.
  /// \param subject who to greet
  Greeter(const char* subject) : subject(subject) {}

  /// Greets the caller.
  /// \return a warming welcome message
  std::string greet() const;

 private:
  const std::string subject;
};
};
