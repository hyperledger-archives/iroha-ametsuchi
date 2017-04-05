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

#include <ametsuchi/currency.h>
#include <cstdint>

namespace ametsuchi {

Currency::Currency(uint64_t amount, uint8_t precision)
    : amount_(amount), precision_(precision), div_(1) {
  for (uint8_t i = 0; i < precision_; i++) {
    div_ *= 10;
  }
}

Currency Currency::operator+(const Currency &a, const Currency &b) {
  return Currency(a.amount_ + b.amount_, precision_);
}

Currency Currency::operator-(const Currency &a, const Currency &b) {
  return Currency(a.amount_ - b.amount_, precision_);
}

Currency Currency::operator+(const Currency &a, uint64_t num) {
  return Currency(a.amount_ + num, precision_);
}

Currency Currency::operator-(const Currency &a, uint64_t num) {
  return Currency(a.amount_ - num, precision_);
}

Currency Currency::operator-(const Currency &a) {
  return Currency(amount_ - a.amount_, precision_);
}

bool Currency::operator<(const Currency &a) {
  //TODO: test is needed
  return (integer() < a.integer()) || (integer() == a.integer() && fractional() < a.fractional());
}

std::string Currency::to_string() {
  return std::to_string(this->integer()) + "." +
         std::to_string(this->fractional());
}

uint64_t Currency::integer() const { return amount_ / div_; }

uint64_t Currency::fractional() const { return amount_ % div_; }

uint64_t Currency::get_amount() const { return amount_; }

uint8_t Currency::get_precision() const { return precision_; }

}  // namespace ametsuchi
