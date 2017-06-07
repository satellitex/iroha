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

#ifndef __TIME_HPP_
#define __TIME_HPP_

#include <cstdint>
#include <string>
#include <array>

namespace datetime {

constexpr size_t BYTE_ARRAY_SIZE = 25;
using TimeStamp = std::array<uint8_t, BYTE_ARRAY_SIZE>;

std::uint64_t unixtime();

TimeStamp unixtimeByteArray();

std::string dateStr();

std::string unixtime2date(std::int64_t unixtime);

};  // namespace datetime

#endif
