#include <ametsuchi/command.h>

namespace ametsuchi {
Command::Command(const std::string &filename) : filename(filename) {
  auto openmode = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  auto errcode = sqlite3_open_v2(filename.c_str(), &db, openmode, nullptr);
  error = (errcode != SQLITE_OK);
}

~Command::Command() {
  // sqlite3_close_v2 should not be used
  sqlite3_close(db);
  // TODO: Log errcode
  // auto errcode = sqlite3_close(db);
}

bool Command::Error() const { return error; }

}
