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

#include <utils/datetime.hpp>
#include "block_builder.hpp"
#include <stdexcept> // runtime_error
#include <array> // timestamp

#include <flatbuffers/flatbuffers.h>
#include <main_generated.h>
#include <service/flatbuffer_service.hpp> // verifier::VerifyBlock

namespace sumeragi {

flatbuffers::Offset<protocol::Signature> Signature::packOffset(
  flatbuffers::FlatBufferBuilder &fbb) const {
  return protocol::CreateSignature(
    fbb, fbb.CreateVector(publicKey), fbb.CreateVector(signature)
  );
}

void Signature::unpackSignature(const std::vector<uint8_t>& flatbuf) {
  auto root = flatbuffers::GetRoot<protocol::Signature>(flatbuf.data());
  auto pk = std::vector<uint8_t>(root->pubkey()->begin(), root->pubkey()->end());
  auto sig = std::vector<uint8_t>(root->sig()->begin(), root->sig()->end());
  *this = sumeragi::Signature(std::move(pk), std::move(sig));
}

flatbuffers::BufferRef<protocol::Block> Block::packBufferRef(flatbuffers::FlatBufferBuilder& fbb) const {
  // Create tx wrapper offset vector
  std::vector<flatbuffers::Offset<protocol::TransactionWrapper>> txs_o;
  for (const auto& tx: txs) {
    auto root = flatbuffers::GetRoot<protocol::TransactionWrapper>(tx.data());
    std::vector<uint8_t> txbuf(root->tx()->begin(), root->tx()->end());
    txs_o.push_back(
      protocol::CreateTransactionWrapper(fbb, fbb.CreateVector(txbuf))
    );
  }

  // Create peer sigs offset vector
  std::vector<flatbuffers::Offset<protocol::Signature>> peer_sigs_o;
  for (const auto& sig: peer_sigs) {
    peer_sigs_o.push_back(sig.packOffset(fbb));
  }

  auto t = datetime::unixtimeByteArray();
  std::vector<uint8_t> vstamp(t.begin(), t.end());

  auto offset = protocol::CreateBlock(
    fbb, fbb.CreateVector(txs_o),
    fbb.CreateVector(peer_sigs_o),
    /* prev_hash */ 0,
    /* length */ 0,
    /* merkle_root */ 0,
    /* height */ 0,
    fbb.CreateVector(vstamp),
    protocol::BlockState::UNCOMMITTED
  );

  fbb.Finish(offset);
  return flatbuffers::BufferRef<protocol::Block>(
    fbb.GetBufferPointer(), fbb.GetSize()
  );
}

void Block::unpackBlock(const uint8_t* flatbuf, size_t length) {
  auto valid = flatbuffer_service::verifier::VerifyBlock(flatbuf, length);
  if (!valid) {
    throw std::runtime_error("Block validation failed");
  }

  auto root = flatbuffers::GetRoot<protocol::Block>(flatbuf);
  std::vector<std::vector<uint8_t>> txs;
  for (const auto& txw: *root->txs()) {
    txs.emplace_back(
      txw->tx()->begin(),
      txw->tx()->end()
    );
  }
  *this = Block {
    std::move(txs),
    {},
    datetime::unixtimeByteArray(),
    sumeragi::BlockState::UNCOMMITTED
  };
}

void Block::unpackBlock(const std::vector<uint8_t>& flatbuf) {
  unpackBlock(flatbuf.data(), flatbuf.size());
}

BlockBuilder::BlockBuilder() {}
BlockBuilder& BlockBuilder::setTxs(const std::vector<std::vector<uint8_t>>& txs)
{
  if (txs.size() > MAX_TXS) {
    throw std::runtime_error("overflow txs.size");
  }
  if (txs.empty()) {
    throw std::runtime_error("doesn't contain any tx");
  }
  txs_ = txs;
  buildStatus_ |= BuildStatus::INIT_WITH_TXS;
  return *this;
}

BlockBuilder& BlockBuilder::setBlock(const Block& block)
{
  block_ = block;
  buildStatus_ |= BuildStatus::INIT_WITH_BLOCK;
  return *this;
}

BlockBuilder& BlockBuilder::addSignature(const Signature& sig) {
  if (!(buildStatus_ & INIT_WITH_BLOCK)) {
    throw std::runtime_error("not initialized with setBlock()");
  }
  if (buildStatus_ & BuildStatus::ATTACHED_SIGNATURE) {
    throw std::runtime_error("duplicate addSignature()");
  }
  peer_sigs_.push_back(sig);
  return *this;
}

Block BlockBuilder::build() {

  if (buildStatus_ & BuildStatus::INIT_WITH_TXS) {
    if (buildStatus_ & BuildStatus::COMPLETE_FROM_TXS != buildStatus_) {
      throw std::runtime_error("not completed init with txs");
    }
    return Block {
      std::move(txs_),
      {},
      datetime::unixtimeByteArray(),
      BlockState::UNCOMMITTED
    };
  }
  else if (buildStatus_ & BuildStatus::INIT_WITH_BLOCK) {
    if (buildStatus_ & BuildStatus::COMPLETE_FROM_BLOCK != buildStatus_) {
      throw std::runtime_error("not completed init with block");
    }
    return block_;
  }
  else {
    throw std::runtime_error("not completed any build");
  }
}

Block BlockBuilder::buildCommit() {
  if (buildStatus_ & BuildStatus::INIT_WITH_BLOCK) {
    if (buildStatus_ != BuildStatus::COMMITTED) {
      throw std::runtime_error("should be committed build status");
    }
    block_.state = BlockState::COMMITTED;
    return block_;
  }
}

}  // namespace sumeragi
