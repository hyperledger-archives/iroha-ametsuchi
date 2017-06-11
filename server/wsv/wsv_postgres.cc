#include <cstdint>
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

#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include "factory.h"
#include "manager.h"
#include "wsv.h"

namespace wsv {

class WSVPostgres : public WSV {
 public:
  WSVPostgres() {
    pqxx::work txn(connection_);
    txn.exec(init_);
    txn.commit();
  }

  ~WSVPostgres() { connection_.disconnect(); }

  void add_account(std::string account_id, uint8_t quorum,
                   uint32_t status) override {
    pqxx::work txn(connection_);
    try {
      txn.exec(
          "INSERT INTO public.account(\n"
          "            account_id, quorum, status)\n"
          "    VALUES (" +
          txn.quote(account_id) + ", " + txn.quote((uint32_t)quorum) + // TODO fix pqxx
          ", " + txn.quote(status) + ");");
      txn.commit();
    } catch (std::exception e) {
      std::cerr << e.what() << std::endl;
    }
  }


  void add_peer(std::string account_id, std::string address, uint32_t state) override {
    pqxx::work txn(connection_);
    try {
      txn.exec(
          "INSERT INTO public.peer(\n"
          "            account_id, address, state)\n"
          "    VALUES (" +
          txn.quote(account_id) + ", " + txn.quote(address) +
          ", " + txn.quote(state) + ");");
      txn.commit();
    } catch (std::exception e) {
      std::cerr << e.what() << std::endl;
    }
  }


  void add_signatory(std::string account_id,
                     std::string public_key) override {
    pqxx::work txn(connection_);
    try {
      txn.exec(
          "INSERT INTO public.signatory(\n"
          "            account_id, public_key)\n"
          "    VALUES (" +
          txn.quote(account_id) + ", " + txn.quote(public_key) +
          ");");
      txn.commit();
    } catch (std::exception e) {
      std::cerr << e.what() << std::endl;
    }
  }


  std::vector<std::string> get_peers() override {
    pqxx::work txn(connection_);
    pqxx::result result;
    try {
      result = txn.exec(
          "SELECT \n"
          "  peer.address\n"
          "FROM \n"
          "  public.peer\n"
          "ORDER BY\n"
          "  peer.peer_id ASC;");
      txn.commit();
    } catch (std::exception e) {
      std::cerr << e.what() << std::endl;
    }
    std::vector<std::string> peers;
    for (const auto &i : result) {
      peers.push_back(i["address"].as<std::string>());
    }
    return peers;
  }

 private:
  pqxx::connection connection_;
  std::string init_ = "";
};

class PostgresFactory : public Factory {
 public:
  PostgresFactory() { Manager::instance().insert(*this); }
  std::string name() const override { return "Postgres"; }
  std::unique_ptr<WSV> create_instance() override {
    return std::make_unique<WSVPostgres>();
  }
};

static PostgresFactory postgresFactory;
}
