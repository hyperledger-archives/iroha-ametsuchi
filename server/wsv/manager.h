/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
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

#ifndef AMETSUCHI_MANAGER_H
#define AMETSUCHI_MANAGER_H

#include "factory.h"
#include "wsv.h"
namespace wsv {

class Manager {
 public:
  static Manager& instance();
  virtual void insert(Factory& factory) = 0;
  virtual void erase(Factory& factory) = 0;
  virtual std::unique_ptr<WSV> make_WSV(const std::string& name) = 0;
};

} // namespace wsv

#endif //AMETSUCHI_MANAGER_H