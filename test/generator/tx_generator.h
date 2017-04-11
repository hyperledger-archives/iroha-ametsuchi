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

#ifndef AMETSUCHI_TX_GENERATOR_H
#define AMETSUCHI_TX_GENERATOR_H

#include <ametsuchi/generated/transaction_generated.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace generator {

std::string LEDGER_ = "ledger_default";
std::string DOMAIN_ = "domain_default";
size_t PUB_KEY_LENGTH_ = 44;
size_t SIGNATURE_LENGTH_ = 88;
unsigned int SEED_ = 1337;
const char BASE64_ALPHABET_[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * returns a number in a range [min, max)
 */
int64_t random_number(int64_t min, int64_t max) {
  return min + (rand_r(&SEED_) % (max - min));
}

uint8_t random_printable_char() { return (uint8_t)random_number(32, 126 + 1); }


std::string random_printable_string(size_t length) {
  std::string s;
  std::generate_n(s.begin(), length, random_printable_char);
  return s;
}


std::string random_base64_string(size_t length) {
  std::string s;
  std::generate_n(s.begin(), length, [&BASE64_ALPHABET_]() {
    auto i = generator::random_number(0, sizeof(BASE64_ALPHABET_));
    return (char)BASE64_ALPHABET_[i];
  });

  while (s.size() % 4 == 0) s.push_back('=');
  return s;
}


std::string random_public_key() {
  return random_base64_string(PUB_KEY_LENGTH_);
}


std::vector<uint8_t> random_blob(size_t length) {
  std::vector<uint8_t> v(length);
  std::generate_n(v.begin(), length, std::bind(random_number, 0, 256));
  return v;
}


std::string random_ip() {
  std::string s;
  s += std::to_string(random_number(0, 256));
  s += '.';
  s += std::to_string(random_number(0, 256));
  s += '.';
  s += std::to_string(random_number(0, 256));
  s += '.';
  s += std::to_string(random_number(0, 256));
  return s;
}


std::vector<uint8_t> random_account(const char* pubkey = nullptr,
                                    const char* alias = nullptr) {
  flatbuffers::FlatBufferBuilder fbb(2048);
  auto accpubk = random_public_key();
  auto alias_ = random_printable_string(5);
  ushort n = (ushort)random_number(1, 5);
  std::vector<std::string> signatories;
  std::generate_n(signatories.begin(), n, random_public_key);

  auto account =
    iroha::CreateAccount(fbb, fbb.CreateString(pubkey ? pubkey : accpubk),
                         fbb.CreateString(alias ? alias : alias_),
                         fbb.CreateVectorOfStrings(signatories), n);

  fbb.Finish(account);

  uint8_t* ptr = fbb.GetBufferPointer();
  return {ptr, ptr + fbb.GetSize()};
}


std::vector<uint8_t> random_peer(const char* pubkey = nullptr,
                                 const char* ip = nullptr) {
  flatbuffers::FlatBufferBuilder fbb(2048);

  auto pubk_ = random_public_key();
  auto ip_ = random_ip();
  auto trust = static_cast<double>(random_number(0, 10));

  auto peer = iroha::CreatePeer(fbb, fbb.CreateString(pubkey ? pubkey : pubk_),
                                fbb.CreateString(ip ? ip : ip_), trust);

  fbb.Finish(peer);

  uint8_t* ptr = fbb.GetBufferPointer();
  return {ptr, ptr + fbb.GetSize()};
}

std::vector<uint8_t> random_currency(const uint64_t* amount = nullptr,
                                     const uint8_t* precision = nullptr,
                                     const char* currency_name = nullptr,
                                     const char* domain_name = DOMAIN_.c_str(),
                                     const char* ledger_name = LEDGER_.c_str(),
                                     const char* description = nullptr) {
  flatbuffers::FlatBufferBuilder fbb(2048);

  auto desc = random_printable_string(10);
  auto curn_ = random_printable_string(6);
  uint64_t amount_ = (uint64_t)random_number(0, 1 << 20);
  uint8_t prec_ = (uint8_t)random_number(0, 5);

  auto currency = iroha::CreateCurrency(
    fbb, fbb.CreateString(currency_name ? *currency_name : curn_),
    fbb.CreateString(domain_name), fbb.CreateString(ledger_name),
    fbb.CreateString(description ? description : desc),
    amount ? *amount : amount_, precision ? *precision : prec_);

  fbb.Finish(currency);

  uint8_t* ptr = fbb.GetBufferPointer();
  return {ptr, ptr + fbb.GetSize()};
}


std::vector<uint8_t> random_currency(const uint64_t amount,
                                     const uint8_t precision,
                                     const char* currency_name = nullptr,
                                     const char* domain_name = DOMAIN_.c_str(),
                                     const char* ledger_name = LEDGER_.c_str(),
                                     const char* description = nullptr) {
  return random_currency(&amount, &precision, currency_name, domain_name,
                         ledger_name, description);
}


std::vector<uint8_t> random_AccountAdd_transaction(
  const char* creator = nullptr,
  const std::vector<uint8_t>* account = nullptr) {
  auto creator_ = random_public_key();
  auto account_ = random_account();

  flatbuffers::FlatBufferBuilder fbb(2048);

  // TODO
  // auto tx = iroha::CreateTransaction(fbb, fbb.CreateString(creator? creator))
  return nullptr;
}
}  // namespace generator

#endif  // AMETSUCHI_TX_GENERATOR_H
