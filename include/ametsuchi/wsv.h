#pragma once

#include <sqlite3.h>
#include <string>

namespace ametsuchi {
/**
 * WSV (World State View) represents the current ledger state.
 */
class WSV {
 public:
  WSV(const std::string &filename = "");
  bool Open();
  bool Close();
  std::string Error() const;

 private:
  const std::string filename;
  sqlite3 *db;
  std::string error;

  // Mind the semicolons and commas!
  // Some field names are suffixed with "_"
  // because they clash with SQL keywords.
  const std::string kCreateSchemaSQL =

      "CREATE TABLE IF NOT EXISTS AccountRoot ("
      "account_id INTEGER PRIMARY KEY,"
      "ledger_add INTEGER,"
      "ledger_remove INTEGER"
      ");"

      "CREATE TABLE IF NOT EXISTS AccountLedger ("
      "ledger_id INTEGER,"
      "account_id INTEGER,"
      "domain_add INTEGER,"
      "domain_remove INTEGER,"
      "peer_add INTEGER,"
      "peer_remove INTEGER,"
      "user_add INTEGER,"
      "user_remove INTEGER,"
      "user_give_permission INTEGER,"
      "PRIMARY KEY (ledger_id, account_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS Ledger ("
      "id INTEGER PRIMARY KEY,"
      "name TEXT"
      ");"

      "CREATE TABLE IF NOT EXISTS Asset ("
      "id INTEGER PRIMARY KEY,"
      "domain_id INTEGER,"
      "ledger_id INTEGER,"
      "description TEXT"
      ");"

      "CREATE TABLE IF NOT EXISTS Peer ("
      "id INTEGER,"
      "pubKey TEXT,"
      "ip TEXT,"
      "trust INTEGER,"
      "active INTEGER,"
      "join_network INTEGER,"
      "join_validation INTEGER"
      ");"

      "CREATE TABLE IF NOT EXISTS AccountAssets ("
      "account_id INTEGER,"
      "asset_id INTEGER,"
      "amount INTEGER,"
      "precision INTEGER,"
      "transfer INTEGER,"
      "add_ INTEGER,"
      "remove INTEGER,"
      "create_ INTEGER,"
      "PRIMARY KEY (account_id, asset_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS Domain ("
      "id INTEGER PRIMARY KEY,"
      "name TEXT"
      ");"

      "CREATE TABLE IF NOT EXISTS Signatories ("
      "account_id_1 INTEGER,"
      "account_id_2 INTEGER,"
      "PRIMARY KEY (account_id_1, account_id_2)"
      ");"

      "CREATE TABLE IF NOT EXISTS AccountDomain ("
      "domain_id INTEGER,"
      "account_id INTEGER,"
      "user_give_permission INTEGER,"
      "user_add INTEGER,"
      "user_remove INTEGER,"
      "PRIMARY KEY (domain_id, account_id)"
      ");"

      "";
};
}
