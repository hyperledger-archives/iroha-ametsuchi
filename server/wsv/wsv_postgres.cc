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

#include "wsv_postgres.h"

namespace wsv {

const auto init =
    "CREATE TABLE IF NOT EXISTS account (\n"
    "    account_id uuid PRIMARY KEY,\n"
    "    name character varying(255) NOT NULL,\n"
    "    quorum \"char\" NOT NULL\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS signatory (\n"
    "    account_id uuid NOT NULL REFERENCES account,\n"
    "    public_key bit(256) NOT NULL,\n"
    "    PRIMARY KEY (account_id, public_key)\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS domain (\n"
    "    domain_id uuid PRIMARY KEY,\n"
    "    name character varying(45) NOT NULL,\n"
    "    root_account_id uuid NOT NULL REFERENCES account\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS asset (\n"
    "    asset_id uuid PRIMARY KEY,\n"
    "    name character varying(45) NOT NULL,\n"
    "    domain_id uuid NOT NULL REFERENCES domain\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS account_has_asset (\n"
    "    account_id uuid NOT NULL REFERENCES account,\n"
    "    asset_id uuid NOT NULL,\n"
    "    amount character varying(256) NOT NULL,\n"
    "    \"precision\" \"char\" NOT NULL,\n"
    "    permissions bit varying NOT NULL,\n"
    "    PRIMARY KEY (account_id, asset_id)\n"
    ");\n"
    "CREATE TABLE IF NOT EXISTS domain_has_account (\n"
    "    domain_id uuid NOT NULL REFERENCES domain,\n"
    "    account_id uuid NOT NULL REFERENCES account,\n"
    "    permissions bit varying NOT NULL,\n"
    "    PRIMARY KEY (domain_id, account_id)\n"
    ");";

const auto drop =
    "DROP TABLE IF EXISTS domain_has_account;\n"
    "DROP TABLE IF EXISTS account_has_asset;\n"
    "DROP TABLE IF EXISTS asset;\n"
    "DROP TABLE IF EXISTS domain;\n"
    "DROP TABLE IF EXISTS signatory;\n"
    "DROP TABLE IF EXISTS account;";

bool WSVPostgres::add_account(uint64_t account_id, std::string name) {
  pqxx::work txn(connection_);
  char buf[33];
  sprintf(buf, "%032lu", account_id);
  txn.exec(
      "INSERT INTO public.account("
      "account_id, name, quorum)"
      "VALUES (" +
      txn.quote(buf) + ", " + txn.quote(name) + ", '1')");
  txn.commit();
  return true;
}
WSVPostgres::WSVPostgres() : connection_() {
  pqxx::work txn(connection_);
  txn.exec(init);
  txn.commit();
}
void WSVPostgres::drop_tables() {
  pqxx::work txn(connection_);
  txn.exec(drop);
  txn.commit();
}
bool WSVPostgres::add_balance(uint64_t account_id, std::uint64_t amount) {
  return false;
}
uint64_t WSVPostgres::get_balance_by_account_id(uint64_t account_id) {
  return 0;
}
std::string WSVPostgres::get_account_by_id(uint64_t account_id) {
  pqxx::work txn(connection_);
  char buf[33];
  sprintf(buf, "%032lu", account_id);
  auto res = txn.exec(
      "SELECT "
      "account.name "
      "FROM "
      "public.account "
      "WHERE "
      "account.account_id = " +
      txn.quote(buf) + ";");
  txn.commit();
  return res.at(0)["name"].as<std::string>();
}
bool WSVPostgres::add_domain(uint64_t domain_id, std::string name,
                             uint64_t root_account_id) {
  pqxx::work txn(connection_);
  char domain_uuid[33], root_account_uuid[33];
  sprintf(domain_uuid, "%032lu", domain_id);
  sprintf(root_account_uuid, "%032lu", root_account_id);
  txn.exec(
      "INSERT INTO public.domain(\n"
      "domain_id, name, root_account_id)\n"
      "VALUES (" +
      txn.quote(domain_uuid) + ", " + txn.quote(name) + ", " +
      txn.quote(root_account_uuid) + ");");
  txn.commit();
  return true;
}
bool WSVPostgres::add_asset(uint64_t asset_id, std::string name,
                            uint64_t domain_id) {
  pqxx::work txn(connection_);
  char asset_uuid[33], domain_uuid[33];
  sprintf(asset_uuid, "%032lu", asset_id);
  sprintf(domain_uuid, "%032lu", domain_id);
  txn.exec(
      "INSERT INTO public.asset(\n"
      "asset_id, name, domain_id)\n"
      "VALUES (" +
      txn.quote(asset_uuid) + ", " + txn.quote(name) + ", " +
      txn.quote(domain_uuid) + ");");
  txn.commit();
  return true;
}
bool WSVPostgres::add_balance(uint64_t account_id, uint64_t asset_id,
                              std::uint64_t amount) {
  pqxx::work txn(connection_);
  char account_uuid[33], asset_uuid[33];
  sprintf(account_uuid, "%032lu", account_id);
  sprintf(asset_uuid, "%032lu", asset_id);
  auto res = txn.exec(
      "SELECT "
      "account_has_asset.amount "
      "FROM "
      "public.account_has_asset "
      "WHERE "
      "account_has_asset.account_id = " +
      txn.quote(account_uuid) +
      " AND \n"
      "account_has_asset.asset_id = " +
      txn.quote(asset_uuid) + ";");
  uint64_t current_amount = 0;
  if (res.size()) {
    current_amount = std::stoull(res.at(0)["amount"].as<std::string>());
  }
  current_amount += amount;
  txn.exec(
      "INSERT INTO public.account_has_asset(\n"
      "account_id, asset_id, amount, \"precision\", permissions)\n"
      "VALUES (" +
      txn.quote(account_uuid) + ", " + txn.quote(asset_uuid) + ", " +
      txn.quote(current_amount) +
      ", '0', '0') "
      "ON CONFLICT (account_id, asset_id) \n"
      "DO UPDATE SET\n"
      "amount = EXCLUDED.amount;");
  txn.commit();
  return true;
}
uint64_t WSVPostgres::get_balance_by_account_id_asset_id(uint64_t account_id,
                                                         uint64_t asset_id) {
  pqxx::work txn(connection_);
  char account_uuid[33], asset_uuid[33];
  sprintf(account_uuid, "%032lu", account_id);
  sprintf(asset_uuid, "%032lu", asset_id);
  auto res = txn.exec(
      "SELECT "
      "account_has_asset.amount "
      "FROM "
      "public.account_has_asset "
      "WHERE "
      "account_has_asset.account_id = " +
      txn.quote(account_uuid) +
      " AND \n"
      "account_has_asset.asset_id = " +
      txn.quote(asset_uuid) + ";");
  txn.commit();
  return std::stoull(res.at(0)["amount"].as<std::string>());
}
}
