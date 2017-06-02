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

#include <crypto/hash.hpp>
#include <crypto/signature.hpp>
#include <crypto/base64.hpp>
#include <service/flatbuffer_service.h>
#include <commands_generated.h>
#include <main_generated.h>
#include <infra/config/peer_service_with_json.hpp>
#include <flatbuffers/flatbuffers.h>
#include <utils/datetime.hpp>
#include <utils/random.hpp>

#include <assert.h>
#include <chrono>
#include <string>
#include <vector>

std::string random_string(int length) {
  std::string ret;
  for (int i=0; i<length; i++) {
    ret += 'a' + rand() % 26;
  }
  return ret;
}

std::vector<uint8_t> CreateSampleTx() {
  flatbuffers::FlatBufferBuilder fbb;

  const auto accountBuf = flatbuffer_service::account::CreateAccount(
    random_string(5), random_string(5), random_string(5), {}, 1);

  const auto signatureOffsets = [&] {
    std::vector<uint8_t> sigblob1;
    return std::vector<flatbuffers::Offset<::iroha::Signature>>{
      ::iroha::CreateSignatureDirect(fbb, random_string(2).c_str(), &sigblob1, 100000)
    };
  }();

  std::vector<uint8_t> _hash;

  const auto attachmentOffset = [&] {
    auto data = std::vector<uint8_t>{'d', 't'};
    return ::iroha::CreateAttachmentDirect(
      fbb, random_string(50).c_str(), &data);
  }();

  const auto txOffset = ::iroha::CreateTransactionDirect(
    fbb, "pk", iroha::Command::AccountAdd,
    ::iroha::CreateAccountAddDirect(fbb, &accountBuf).Union(),
    &signatureOffsets, &_hash, 10000, /*attachmentOffset*/ 0);

  fbb.Finish(txOffset);

  auto ptr = fbb.GetBufferPointer();

  return {ptr, ptr + fbb.GetSize()};
}

struct Block {
  std::vector<std::vector<uint8_t>> txs;
};

auto CreateBlock(int num_of_tx) {
  Block block;
  for (int i = 0; i < num_of_tx; i++) {
    auto tx = CreateSampleTx(); // サイズは小さく作っても300bytes以上
    block.txs.push_back(tx);
  }
  return block;
}

namespace storage {

constexpr int num_of_tx = 1000;
Block block;

}  // namespace storage

class Server {
  void Communicate() {
    // random
  }
};

int main(int argc, char** argv) {

  if (argc != 2) {
    std::cout << "Usage: ./bm_connection ";
    exit(0);
  }

  auto repeat_times = std::stoi(std::string(argv[1]));

  auto keyPair = signature::generateKeyPair();
  auto creatorPubkey = keyPair.publicKey;
  auto creatorPrivateKey = keyPair.privateKey;

  /*
   * N!通りの順序でBlockを投げる
   * 1つのBlockに入れるTxは1000個
   * 順序の末端ノードがファイルに速度を書き出す
   * 速度: 1000(tx数) / 全バリデーションにかかった時間(sec)
   */

  std::cout << "Size of tx = " << CreateBlock(1).txs[0].size() << std::endl;
  storage::block = CreateBlock(num_of_tx);

  // Wake up server.

  return 0;
}
