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
#include <service/protocol_to_string.hpp>
#include <utils/random_generator.hpp>

#define DEF_TEST_ACTION(ACT) \
TEST(protocol_to_string_test, to_string_ ## ACT) { \
  ASSERT_NO_THROW({ \
    flatbuffers::FlatBufferBuilder fbb; \
    auto act = random_generator::random_ ## ACT(fbb); \
    auto acts = std::vector<flatbuffers::Offset<protocol::ActionWrapper>>{ \
      act \
    }; \
    auto att = random_generator::random_attachment(fbb); \
    auto tx = random_generator::random_tx(fbb, acts, att); \
    auto root = flatbuffers::GetRoot<protocol::Transaction>(tx.data()); \
    std::cout << protocol::to_string(root) << std::endl; \
  }); \
}

DEF_TEST_ACTION(AccountCreate)
DEF_TEST_ACTION(AssetCreate)
