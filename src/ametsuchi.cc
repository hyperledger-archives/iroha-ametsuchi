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

#include "ametsuchi.h"

namespace ametsuchi {

std::unique_ptr<Ametsuchi> Ametsuchi::create() {
  return std::make_unique<AmetsuchiImpl>();
}

void AmetsuchiImpl::insert(const std::string &blob) {

}
std::string AmetsuchiImpl::get_by_height(uint32_t height) {
  return nullptr;
}
std::string AmetsuchiImpl::get_by_block_hash(std::string hash) {
  return nullptr;
}
std::string AmetsuchiImpl::get_by_tx_hash(std::string hash) {
  return nullptr;
}
void AmetsuchiImpl::erase_by_height(uint32_t height) {

}
void AmetsuchiImpl::add_account() {

}
void AmetsuchiImpl::add_signatory(std::string account_id, std::string string) {

}
void AmetsuchiImpl::add_peer() {

}
std::vector<std::string> AmetsuchiImpl::get_peers(bool committed) {
  return {};
}
}
