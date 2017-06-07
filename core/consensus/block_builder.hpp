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

#ifndef IROHA_BLOCK_BUILDER_H
#define IROHA_BLOCK_BUILDER_H

#include <utils/datetime.hpp>
#include <vector>
#include <main_generated.h> // protocol::Block, protocol::Signature

// FIXME: Document's required size = 2^32 - 1
constexpr unsigned MAX_TXS = (unsigned)(2 * 1e7); // (1LL << 32) - 1;

namespace sumeragi {

struct Signature {
  std::vector<uint8_t> publicKey;
  std::vector<uint8_t> signature;

  Signature() = default;
  Signature(std::vector<uint8_t> pubkey, std::vector<uint8_t> signature)
    : publicKey(std::move(pubkey)), signature(std::move(signature))
  {}

  flatbuffers::Offset<protocol::Signature> packOffset(flatbuffers::FlatBufferBuilder& fbb) const;
  void unpackSignature(const std::vector<uint8_t>& flatbuf);
};

enum class BlockState {
  COMMITTED,
  UNCOMMITTED
};

struct Block {
  std::vector<std::vector<uint8_t>> txs;
  std::vector<Signature> peer_sigs;
  std::array<uint8_t, datetime::BYTE_ARRAY_SIZE> created;
  BlockState state;

  Block(
    std::vector<std::vector<uint8_t>> txs,
    std::vector<Signature> peer_sigs,
    std::array<uint8_t, datetime::BYTE_ARRAY_SIZE> created,
    BlockState state
  )
    : txs(std::move(txs)),
      peer_sigs(std::move(peer_sigs)),
      created(std::move(created)),
      state(state)
  {}

  Block() = default;

  flatbuffers::BufferRef<protocol::Block> packBufferRef(flatbuffers::FlatBufferBuilder& fbb) const;
  void unpackBlock(const std::vector<uint8_t>& flatbuf);
  void unpackBlock(const uint8_t* flatbuf, size_t length);
};

class BlockBuilder {
public:
  BlockBuilder();
  BlockBuilder& setTxs(const std::vector<std::vector<uint8_t>>& txs);
  BlockBuilder& setBlock(const Block& block);
  BlockBuilder& addSignature(const Signature& sig);
  Block build();
  Block buildCommit();

private:

  enum BuildStatus: uint8_t {
    INIT_WITH_TXS       = 1 << 0,
    INIT_WITH_BLOCK     = 1 << 1,
    ATTACHED_SIGNATURE  = 1 << 2,
    COMPLETE_FROM_TXS   = INIT_WITH_TXS,
    COMPLETE_FROM_BLOCK = INIT_WITH_BLOCK
                        | ATTACHED_SIGNATURE,
    COMMITTED           = INIT_WITH_BLOCK
  };

  int buildStatus_ = 0;

  // initWithTxs
  std::vector <std::vector<uint8_t>> txs_;

  // initWithBlock
  Block block_;
  std::vector <Signature> peer_sigs_;
  BlockState state_;
};

};

#endif //IROHA_BLOCK_BUILDER_H
