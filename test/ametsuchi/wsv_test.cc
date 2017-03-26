#include <ametsuchi/wsv.h>
#include <gtest/gtest.h>

namespace ametsuchi {
TEST(WSVTest, OpenClose) {
  WSV wsv("/tmp/amdb.sqlite");
  bool opened = wsv.Open();
  ASSERT_TRUE(opened) << wsv.Error();
  bool closed = wsv.Close();
  ASSERT_TRUE(closed) << wsv.Error();
}
}
