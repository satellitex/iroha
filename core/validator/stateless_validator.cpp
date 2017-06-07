/**
 * Copyright 2016 Soramitsu Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <flatbuffers/flatbuffers.h>
#include <consensus/block_builder.hpp>
#include <functional>
#include <transaction_generated.h>

#include "stateless_validator.hpp"

namespace validator {
namespace stateless {

std::function<void(const sumeragi::Block&)> dispatchToSumeragi;

/**
 * receive(block)
 * @brief sets block dispatcher if validation succeeded.
 * @param block
 */
void receive(std::function<void(const sumeragi::Block&)> dispatcher) {
  dispatchToSumeragi = dispatcher;
}

bool test_tx(const protocol::Transaction& tx) {
  StatelessValidator<protocol::Transaction> validator;

  validator.addValidator([](const protocol::Transaction& tx) -> bool {
    return true;
  });

  return validator.test(tx);
}

bool test(const sumeragi::Block& block) {

  StatelessValidator<sumeragi::Block> validator;

  // verify block.txs
  validator.addValidator([](const sumeragi::Block& block) -> bool {
    for (auto const& tx: block.txs) {
      // verify tx flatbuf
      flatbuffers::Verifier verifier(tx.data(), tx.size());
      if (!verifier.VerifyBuffer<protocol::Transaction>()) {
        return false;
      }
      // verify the content of tx
      if (!test_tx(*flatbuffers::GetRoot<protocol::Transaction>(tx.data()))) {
        return false;
      }
    }
    return true;
  });

  // verify block.created (simple check)
  validator.addValidator([](const sumeragi::Block& block) -> bool {
    return block.created <= datetime::unixtimeByteArray();
  });

  auto valid = validator.test(block);

  if (valid) {
    if (!dispatchToSumeragi) {
      throw std::runtime_error("Not callable 'dispatchToSumeragi'. Set receive() in sumeragi");
    }
    dispatchToSumeragi(block);
    return true;
  }
  return false;
}

} // namespace stateless
} // namespace validator