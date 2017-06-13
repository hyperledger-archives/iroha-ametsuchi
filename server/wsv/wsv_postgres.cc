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

  bool add_account(std::string account_id, uint8_t quorum,
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
      return false;
    }
    return true;
  }


  bool add_peer(const std::string &account_id,
                const std::string &address,
                uint32_t state) override {
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
      return false;
    }
    return true;
  }


  bool add_signatory(const std::string &account_id,
                     const std::string &public_key) override {
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
      return false;
    }
    return true;
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
  const std::string init_ = "CREATE TABLE IF NOT EXISTS account (\n"
    "    account_id char(32) PRIMARY KEY,\n"
    "    quorum int NOT NULL,\n"
    "    status int NOT NULL DEFAULT 0\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS signatory (\n"
    "    account_id char(32) NOT NULL REFERENCES account,\n"
    "    public_key char(32) NOT NULL,\n"
    "    PRIMARY KEY (account_id, public_key)\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS peer (\n"
    "    peer_id serial PRIMARY KEY,\n"
    "    account_id char(32) NOT NULL REFERENCES account,\n"
    "    address inet NOT NULL UNIQUE,\n"
    "    state int NOT NULL DEFAULT 0\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS domain (\n"
    "    domain_id character varying(164) PRIMARY KEY,\n"
    "    parent_domain_id character varying(131) NOT NULL REFERENCES domain(domain_id),\n"
    "    open bool NOT NULL\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS asset (\n"
    "    asset_id character varying(197) PRIMARY KEY,\n"
    "    domain_id character varying(164) NOT NULL REFERENCES domain,\n"
    "    data json\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS exchange (\n"
    "    asset1_id character varying(197) NOT NULL REFERENCES asset(asset_id),\n"
    "    asset2_id character varying(197) NOT NULL REFERENCES asset(asset_id),\n"
    "    asset1 int NOT NULL,\n"
    "    asset2 int NOT NULL,\n"
    "    PRIMARY KEY (asset1_id, asset2_id)\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS wallet (\n"
    "    wallet_id uuid PRIMARY KEY,\n"
    "    asset_id character varying(197),\n"
    "    amount int NOT NULL,\n"
    "    precision int NOT NULL,\n"
    "    permissions bit varying NOT NULL\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS account_has_wallet (\n"
    "    account_id char(32) NOT NULL REFERENCES account,\n"
    "    wallet_id uuid NOT NULL REFERENCES wallet,\n"
    "    permissions bit varying NOT NULL,\n"
    "    PRIMARY KEY (account_id, wallet_id)\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS account_has_asset (\n"
    "    account_id char(32) NOT NULL REFERENCES account,\n"
    "    asset_id character varying(197) NOT NULL REFERENCES asset,\n"
    "    permissions bit varying NOT NULL,\n"
    "    PRIMARY KEY (account_id, asset_id)\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS domain_has_account (\n"
    "    domain_id character varying(164) NOT NULL REFERENCES domain,\n"
    "    account_id char(32) NOT NULL REFERENCES account,\n"
    "    permissions bit varying NOT NULL,\n"
    "    PRIMARY KEY (domain_id, account_id)\n"
    ");";
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
