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

flatbuffers::Offset<::iroha::Signature> CreateSignature(
  flatbuffers::FlatBufferBuilder &fbb, const std::string &hash, uint64_t timestamp) {
  const auto signature = signature::sign(
    hash,
    base64::decode(config::PeerServiceConfig::getInstance().getMyPublicKey()),
    base64::decode(config::PeerServiceConfig::getInstance().getMyPrivateKey()));
  return ::iroha::CreateSignatureDirect(
    fbb, config::PeerServiceConfig::getInstance().getMyPublicKey().c_str(),
    &signature, timestamp);
}

std::vector<uint8_t> CreateSignature(std::string const& txHash) {
  flatbuffers::FlatBufferBuilder fbb;
  fbb.Finish(CreateSignature(fbb, txHash, 12345678));
  auto ptr = fbb.GetBufferPointer();
  return {ptr, ptr + fbb.GetSize()};
}

auto dump(::iroha::Transaction const& tx) {
  return flatbuffer_service::dump(tx);
}

auto CreateTxHash(std::vector<uint8_t> const& tx) {
  return hash::sha3_256_hex(
    dump(*flatbuffers::GetRoot<::iroha::Transaction>(tx.data()))
  );
}

struct Block {
  std::vector<uint8_t> tx;
  std::vector<uint8_t> signature;
};

auto CreateBlock() {
  auto tx = CreateSampleTx(); // サイズは小さく作っても300bytes以上
  Block block;
  block.tx = tx;
  block.signature = CreateSignature(CreateTxHash(tx));
  return block;
}

int num_of_blocks;
std::vector<Block> blocks;
std::vector<double> results;

void process() {
  auto start = std::chrono::high_resolution_clock::now();

  int validation_failure = 0;

  for (int i = 0; i < num_of_blocks; i++) {
    auto txHash = CreateTxHash(blocks[i].tx);
    auto sig = flatbuffers::GetRoot<::iroha::Signature>(blocks[i].signature.data());

    auto pk64 = sig->publicKey()->str();
    auto pkbytes = base64::decode(pk64);

    std::vector<uint8_t> sigbytes(
      sig->signature()->begin(),
      sig->signature()->end()
    );
    if (!signature::verify(sigbytes, txHash, pkbytes)) { validation_failure++; }
  }

  auto end = std::chrono::high_resolution_clock::now();

  if (validation_failure) {
    std::cerr << "validation failure: " << validation_failure << " / " << num_of_blocks << std::endl;
  }

  std::chrono::duration<double> diff = end-start;
  std::cout << diff.count() << "\n";
  results.push_back(diff.count());
}

int main(int argc, char** argv) {

  if (argc != 2) {
    std::cout << "Usage: ./bm_validation [num_of_blocks]\n";
    exit(0);
  }

  num_of_blocks = std::stoi(std::string(argv[1]));

  /*
   * 100万個の電子署名をバリデーションする
   * 時間を標準出力する
   */

  std::cout << "Size of tx = " << CreateBlock().tx.size() << std::endl;

  for (int i = 0; i < num_of_blocks; i++) {
    blocks.push_back(CreateBlock());
  }

  constexpr int TryTimes = 5;

  std::cout << "Calc validation time (sec) " << TryTimes << " times.\n";

  for (int i = 0; i < TryTimes; i++) {
    process();
  }

  std::cout << "\n";
  std::sort(results.begin(), results.end());
  std::cout << "Result CSV (sorted):\n";

  for (size_t i = 0; i < results.size(); i++) {
    if (i) std::cout << ",";
    std::cout << results[i];
  }

  std::cout << std::endl;

  return 0;
}
