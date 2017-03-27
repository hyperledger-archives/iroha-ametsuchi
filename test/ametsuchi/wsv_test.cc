#include <ametsuchi/wsv.h>
#include <gtest/gtest.h>

namespace ametsuchi {
TEST(WSVTest, OpenClose) {
  WSV wsv;
  bool opened = wsv.Open();
  ASSERT_TRUE(opened) << wsv.Error();
  bool closed = wsv.Close();
  ASSERT_TRUE(closed) << wsv.Error();
}
}
