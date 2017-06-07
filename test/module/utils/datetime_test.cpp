/**
 * Copyright Soramitsu Co., Ltd. 2016 All Rights Reserved.
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

#include <gtest/gtest.h>
#include <utils/datetime.hpp>
#include <string>

TEST(datetime_test, unixtimeByteArray) {
  // e.g. [0,0,0,0,1,4,3,..., 8] (25bytes)
  constexpr size_t BYTE_ARRAY_SIZE = 25;
  ASSERT_EQ(datetime::BYTE_ARRAY_SIZE, BYTE_ARRAY_SIZE);

  auto time = datetime::unixtime();
  auto timeBytes = datetime::unixtimeByteArray();
  auto timeStr = std::to_string(time);

  for (size_t i = timeStr.size(); i < BYTE_ARRAY_SIZE; i++) {
    timeStr = "0" + timeStr;
  }

  ASSERT_EQ(timeStr.size(), BYTE_ARRAY_SIZE);
  ASSERT_EQ(timeBytes.size(), BYTE_ARRAY_SIZE);

  std::string actual;
  for (size_t i = 0; i < BYTE_ARRAY_SIZE; i++) {
    actual.push_back((char)(timeBytes[i] + '0'));
  }
  ASSERT_STREQ(actual.c_str(), timeStr.c_str());
}
