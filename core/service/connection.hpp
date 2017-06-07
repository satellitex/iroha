/**
 * Copyright 2017 Soramitsu Co., Ltd.
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

#ifndef IROHA_SERVICE_CONNECTION_HPP
#define IROHA_SERVICE_CONNECTION_HPP

#include <functional>

namespace sumeragi {
class Block;
}

namespace connection {
namespace with_sumeragi {

/**
 * unicast()
 * @brief unicasts block to a validating peer.
 * @param block - block to consensus.
 * @param index - validating peer's index.
 */
void unicast(const sumeragi::Block& block, ::peer::Nodes::const_iterator iter);

/**
 * multicast()
 * @brief multicasts block to [beginIndex, endIndex) peers.
 * @param block - block to consensus.
 * @param begin - validating peer's iterator except leader (usually next to begin())
 * @param end - validating peer's tail + 1 iterator
 */
void multicast(const sumeragi::Block& block,
               ::peer::Nodes::const_iterator begin,
               ::peer::Nodes::const_iterator end);

/**
 * commit()
 * @brief commits block to all peers including sender.
 * @param block - block to consensus.
 */
void commit(const sumeragi::Block& block);

} // namespace with_sumeragi
} // namespace connection

#endif  // IROHA_SERVICE_CONNECTION_HPP
