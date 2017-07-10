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
#include <model/model_crypto_provider_impl.hpp>
#include <crypto/crypto.hpp>

iroha::model::Transaction create_transaction() {
  iroha::model::Transaction tx{};
  memset(tx.creator.data(), 0x1, 32);

  tx.tx_counter = 0;
  tx.created_ts = 0;
  return tx;
}

TEST(CryptoProvider, SignAndVerify){
  // generate privkey/pubkey keypair
  auto seed = iroha::create_seed();
  auto keypair = iroha::create_keypair(seed);

  auto model_tx = create_transaction();

  iroha::model::ModelCryptoProviderImpl crypto_provider(keypair.privkey, keypair.pubkey);
  crypto_provider.sign(model_tx);
  ASSERT_TRUE(crypto_provider.verify(model_tx));

  // now modify transaction's meta, so verify should fail
  memset(model_tx.creator.data(), 0x123, iroha::ed25519::pubkey_t::size());
  ASSERT_FALSE(crypto_provider.verify(model_tx));
}