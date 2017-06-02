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

std::vector<std::vector<uint8_t>> txs;
std::vector<std::vector<uint8_t>> sigs;
std::vector<std::thread> threads;
std::mutex mtx;
int currTx;
int validation_failure;

void CreateQueue(int num_of_sigs) {
  for (int i=0; i<num_of_sigs; i++) {
    auto tx = CreateSampleTx();
    auto txHash = CreateTxHash(tx);
    txs.push_back(tx);
    sigs.push_back(CreateSignature(txHash));
  }
}

void ProcessTx() {

  while (true) {
    mtx.lock();
    auto &tx = txs[currTx];
    auto &sig = sigs[currTx];
    currTx++;
    if (currTx >= txs.size()) {
      mtx.unlock();
      return;
    }
    mtx.unlock();

    auto txHash = CreateTxHash(tx);
    auto sigroot = flatbuffers::GetRoot<::iroha::Signature>(sig.data());

    auto pk64 = sigroot->publicKey()->str();
    auto pkbytes = base64::decode(pk64);

    std::vector<uint8_t> sigbytes(
      sigroot->signature()->begin(),
      sigroot->signature()->end()
    );
    if (!signature::verify(sigbytes, txHash, pkbytes)) { validation_failure++; }
  }
}

void Init(int num_of_sigs, int num_of_threads) {
  txs.clear();
  sigs.clear();
  currTx = 0;
  validation_failure = 0;
  threads.clear();
  CreateQueue(num_of_sigs);
}

int main(int argc, char** argv) {

  // blockを100万個作る

  // sign(block.peer_sigs[0].sig, block.peer_sigs[0].pubkey) から expected_hash を作る

  // actual_hash = sha3_256(dump(block.tx))

  // if (actual_hash != expected_hash) { validation_failure ++; }

  /*
   Using 512MB, 2, 4, 8 Gb servers on vultr,
   test time to complete 1 million digital signature verifications in parallel, with 4 threads,
   while storing a 20,000,000 length hashmap in memory of public keys to a Json string
   that is different for every record. No need to plot the violin, but just record total time.
   */


  if (argc != 3) {
    std::cout << "Usage: ./bm_validation_memory [num of sigs] [num of threads]\n";
    exit(0);
  }

  auto num_of_sigs = std::stoi(std::string(argv[1]));
  auto num_of_threads = std::stoi(std::string(argv[2]));

  auto keyPair = signature::generateKeyPair();
  auto creatorPubkey = keyPair.publicKey;
  auto creatorPrivateKey = keyPair.privateKey;

  std::cout << "Size of tx = " << CreateBlock().tx.size() << std::endl;

  std::vector<Block> blocks;
  for (int i = 0; i < num_of_sigs; i++) {
    blocks.push_back(CreateBlock());
  }

  Init(num_of_sigs, num_of_threads);

  std::cout << "Start\n";

  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < num_of_threads; i++) {
    threads.push_back(std::thread(ProcessTx));
  }

  for (int i = 0; i < num_of_threads; i++) {
    threads[i].join();
  }

  auto end = std::chrono::high_resolution_clock::now();

  if (validation_failure) {
    std::cerr << "success rate: " << (long double) validation_failure / num_of_sigs << std::endl;
  }

  std::chrono::duration<double> diff = end-start;
  std::cout << diff.count() << " sec\n";

  return 0;
}
