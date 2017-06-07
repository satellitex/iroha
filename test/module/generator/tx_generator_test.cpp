/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
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
#include <main_generated.h>
#include "tx_generator.h"

/*
TEST(tx_generator_test, asset_id) {
  // Use regexp
  std::cout << generator::random_asset_id() << std::endl;
}
*/

#define DEF_TEST_CMD(CMD) \
TEST(tx_generator_test, CMD) { \
  ASSERT_NO_THROW({ \
    flatbuffers::FlatBufferBuilder fbb; \
    auto actions = std::vector<flatbuffers::Offset<protocol::ActionWrapper>>{ \
      generator::random_ ## CMD(fbb) \
    }; \
    auto attachment = generator::random_attachment(fbb); \
    auto tx = generator::random_tx(fbb, actions, attachment); \
    flatbuffers::Verifier verifier(tx.data(), tx.size()); \
    if (!verifier.VerifyBuffer<protocol::Transaction>()) { \
      throw std::runtime_error("Failed to verify flatbuf."); \
    } \
    std::cout << dump::toString( \
      *flatbuffers::GetRoot<protocol::Transaction>(tx.data()) \
    ) << std::endl; \
  }); \
}

DEF_TEST_CMD(AccountCreate)
DEF_TEST_CMD(AssetCreate)
