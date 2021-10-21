// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#include <gtest/gtest.h>
#include "common/time/datetime.h"

class DateTest : public testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(DateTest, ShouldWork) {
  common::Date d;
  std::string in, out;
  std::tie(in, out) = GetParam();
  if (out.length() == 0) {
    ASSERT_FALSE(d.parse(in.c_str()));
  } else {
    ASSERT_TRUE(d.parse(in.c_str()));
    ASSERT_EQ(d.format(), out);
  }
}

INSTANTIATE_TEST_SUITE_P(Dates, DateTest, testing::Values(
  std::make_pair("2021-10-10", "2021-10-10"),
  std::make_pair("2021-10-33", ""),
  std::make_pair("2100-2-29", ""), // 闰年
  std::make_pair("2000-2-29", "2000-02-29"), // 平年
  std::make_pair("1-1-1", "0001-01-01"),
  std::make_pair("3000-12-31", "3000-12-31")
));

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}