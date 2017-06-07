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
#include <consensus/block_builder.hpp>
#include <main_generated.h> // pack(), unpack()
#include <utils/datetime.hpp> // TimeStamp (type alias, no need to link datetime.cpp)
//#include <builder/transaction_builder.hpp>
#include <crypto/signature.hpp> // generateKey

using namespace sumeragi;

TEST(block_builder_test, to_block) {

  std::vector<std::vector<uint8_t>> txs(1);

  auto block = BlockBuilder()
    .setTxs(txs)
    .build();

  ASSERT_EQ(block.peer_sigs.size(), 0);
  ASSERT_EQ(block.txs.size(), txs.size());
  ASSERT_EQ(block.state, BlockState::UNCOMMITTED);
}

TEST(block_builder_test, limit_capacity) {
  std::vector<std::vector<uint8_t>> txs(MAX_TXS);

  ASSERT_NO_THROW({
    auto block = BlockBuilder()
      .setTxs(txs)
      .build();
  });
}

TEST(block_builder_test, over_capacity) {
  std::vector<std::vector<uint8_t>> txs(MAX_TXS + 1);

  try {
    auto block = BlockBuilder()
      .setTxs(txs)
      .build();
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(), "overflow txs.size");
  }
}

TEST(block_builder_test, no_tx) {
  std::vector<std::vector<uint8_t>> txs;

  try {
    auto block = sumeragi::BlockBuilder()
      .setTxs(txs)
      .build();
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(), "doesn't contain any tx");
  }
}
/*
TEST(block_builder_test, pack_unpack) {


     creator: Signature (required);
  sigs:    [SignatureWithState];
  created: ulong;   // 64 bit(8 bytes)
  nonce:   uint;    // 32 bit(4 bytes)

  // union vector is supported in C++ only.
  // And, Transaction contains array of objects Action. So, Action needs to be wrapped by ActionWrapper.
  // https://github.com/google/flatbuffers/commit/b0752e179bdbae516125cccacd7aebcfd83033a9
  actions: [ActionWrapper] (required);

  attachment: Attachment;

  {
    builder::SignatureHolder signatureHolder;
    auto keyPair = signature::generateKeyPair();
    signatureHolder.pubkey = keyPair.publicKey;
    signatureHolder.sig = signature::sign("message", signatureHolder.pubkey, signatureHolder.sig);
    flatbuffers::FlatBufferBuilder sbb;
    auto sigOffset = builder::SignatureBuilder(signatureHolder).buildOffset(sbb);
    sbb.Finish(sigOffset);
  }
  auto sigRoot = flatbuffers::GetRoot<protocol::Signature>(sbb.GetBufferPointer());

  std::vector<protocol::Signature> sigs { *sigRoot, *sigRoot, *sigRoot };

  flatbuffers::FlatBufferBuilder fbb;
  builder::TransactionBuilder()
    .setCreator(*sigRoot)
    .setSigs(std::move(sigs))
    .setNonce(1234567)
  std::vector<std::vector<uint8_t>> txs {
    {}
  };

  const std::vector<uint8_t> pubkey {'p', 'k'};
  const std::vector<uint8_t> signature {'s', 'k'};
  const datetime::TimeStamp tstamp {1, 2, 3};
  std::vector<sumeragi::Signature> peer_sigs {
    sumeragi::Signature(std::move(pubkey), std::move(signature))
  };
  sumeragi::Block block(
    txs, peer_sigs, tstamp, sumeragi::BlockState::COMMITTED
  );

  flatbuffers::FlatBufferBuilder fbb;
  auto bufferRef = block.packBufferRef(fbb);
  auto root = bufferRef.GetRoot();

  // TODO: Verify txs. Use txbuilder.
  //std::vector<std::vector<uint8_t>> d_txs;
  //ASSERT_EQ(txs, d_txs);

  std::vector<sumeragi::Signature> d_peer_sigs {

  };
  //ASSERT_EQ(peer_sigs, d_peer_sigs);

  std::vector<uint8_t> d_tstamp(
    root->created()->begin(), root->created()->end()
  );
  std::vector<uint8_t> vtstamp(
    tstamp.begin(), tstamp.end()
  );
  //ASSERT_EQ(d_tstamp, vtstamp);

  ASSERT_GT(static_cast<int>(protocol::BlockState::COMMITTED), 0);
  ASSERT_EQ(root->block_state(), protocol::BlockState::COMMITTED);

  std::vector<uint8_t> blockBuf(
    bufferRef.buf, bufferRef.buf + bufferRef.len
  );

  sumeragi::Block d_block;
  d_block.unpackBlock(blockBuf);
  //ASSERT_EQ(block, d_block);
}

*/