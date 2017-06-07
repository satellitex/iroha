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

#include "datetime.hpp"
#include <ctime>
#include <array> // timestamp

namespace datetime {

std::uint64_t unixtime() {
  return static_cast<std::uint64_t>(std::time(nullptr));
}

TimeStamp unixtimeByteArray() {
  std::array<uint8_t, BYTE_ARRAY_SIZE> ret;
  auto t = std::to_string(unixtime());
  const size_t size = t.size();
  for (size_t i = 0; i < BYTE_ARRAY_SIZE; i++) {
    if (BYTE_ARRAY_SIZE < i + size) {
      ret[i] = 0;
    } else {
      ret[i] = (uint8_t)(t[i + size - BYTE_ARRAY_SIZE] - '0');
    }
  }
  return ret;
}

std::string dateStr() {
  std::time_t result = std::time(nullptr);
  return std::asctime(std::localtime(&result));
}

std::string unixtime2date(time_t unixtime) {
  return std::asctime(std::localtime(&unixtime));
}

};  // namespace datetime
