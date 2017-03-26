#include <ametsuchi/wsv.h>

namespace ametsuchi {
WSV::WSV(const std::string &filename) : filename(filename), db(nullptr) {}

bool WSV::Open() {
  auto errcode = sqlite3_open(filename.c_str(), &db);
  if (errcode != SQLITE_OK) {
    error = "failed to open database file";
    return false;
  }
  char *execerr;
  errcode = sqlite3_exec(db, kCreateSchemaSQL.c_str(), nullptr, nullptr, &execerr);
  if (errcode != SQLITE_OK) {
    error = execerr;
    sqlite3_free(execerr);
    sqlite3_close(db);
    // Closing the DB should't fail because there are no active connections.
    // TODO: Log error if the closing fails for some reason.
    return false;
  }
  return true;
}

bool WSV::Close() {
  auto errcode = sqlite3_close(db);
  if (errcode != SQLITE_OK) {
    error = "failed to close database file";
    return false;
  }
  return true;
}

std::string WSV::Error() const {
  return error;
}
}
