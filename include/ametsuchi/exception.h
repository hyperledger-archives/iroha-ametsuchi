/**
 * Copyright Soramitsu Co., Ltd. 2016 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AMETSUCHI_EXCEPTION_H
#define AMETSUCHI_EXCEPTION_H

#include <string>

namespace ametsuchi {
namespace exception {

class Exception : public std::exception {
 public:
  /** Constructor (C strings).
   *  @param message C-style string error message.
   *                 The string contents are copied upon construction.
   *                 Hence, responsibility for deleting the char* lies
   *                 with the caller.
   */
  explicit Exception(const char* message) : msg_(message) {}

  /** Constructor (C++ STL strings).
   *  @param message The error message.
   */
  explicit Exception(const std::string& message) : msg_(message) {}

  /** Destructor.
   * Virtual to allow for subclassing.
   */
  virtual ~Exception() throw() {}

  /** Returns a pointer to the (constant) error description.
   *  @return A pointer to a const char*. The underlying memory
   *          is in posession of the Exception object. Callers must
   *          not attempt to free the memory.
   */
  virtual const char* what() const throw() { return msg_.c_str(); }

 protected:
  /** Error message.
   */
  std::string msg_;
};


class IOError : public Exception {
 public:
  IOError(const char* message) : Exception(message) {}
};
}
}

#endif  // AMETSUCHI_EXCEPTION_H
