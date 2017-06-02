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
#include <fstream>
#include <sstream>
#include <cstdint>

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


std::vector<std::thread> threads;
std::mutex mtx;

std::vector<Block> blocks;
int num_of_blocks;
int num_of_threads;
int currTx;
int validation_failure;
bool running = true;

void ProcessTx() {

  while (true) {
    mtx.lock();
    auto &tx = blocks[currTx].tx;
    auto &sig = blocks[currTx].signature;
    currTx++;
    if (currTx >= num_of_blocks) {
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

void dump(std::string const& path, std::string const& item_name) {
  std::fstream fs(path);
  std::string line;
  std::vector<std::vector<std::string>> buf;
  while (std::getline(fs, line)) {
    std::stringstream ss(line);
    buf.resize(buf.size() + 1);
    for (std::string s; ss >> s;) {
      buf.back().push_back(s);
    }
  }

  std::vector<std::tuple<int, int64_t>> items;
  int curr = 0;

  for (size_t i = 0; i < buf.size(); i++) {
    for (size_t j = 0; j < buf[i].size(); j++) {
      if (buf[i][j] == item_name) {
        assert(i + 1 < buf.size());
        items.emplace_back(curr++, std::stoll(buf[i + 1][j]));
      }
    }
  }

  const auto items_path = path + "-" + item_name;
  std::cout << "output '" << item_name << "' CSV: " << items_path << std::endl;
  std::ofstream out(items_path);

  for (auto const& e: items) {
    out << std::get<0>(e) << "," << std::get<1>(e) << std::endl;
  }
}

int main(int argc, char** argv) {

  if (argc != 3) {
    std::cout << "Usage: ./bm_validation_memory [num of blocks] [num of threads]\n";
    exit(0);
  }

  std::cout << "Creating Blocks...\n";

  num_of_blocks = std::stoi(std::string(argv[1]));
  num_of_threads = std::stoi(std::string(argv[2]));

  std::cout << "Size of tx = " << CreateBlock().tx.size() << std::endl;

  for (int i = 0; i < num_of_blocks; i++) {
    blocks.push_back(CreateBlock());
  }

  const std::string path = "/tmp/validation_memory";//-" + std::to_string((int)getpid());
  const std::string command = "vmstat >> " + path;

  const std::string rm_cmd  = "rm " + path;
  std::cout << rm_cmd << std::endl;
  system(rm_cmd.c_str());

  constexpr int Interval = 1000;
  std::cout << "Interval: " << (double) Interval / 1000.0 << std::endl;
  std::cout << command << std::endl;
  system(command.c_str());

  std::thread vmstat_logger([command, Interval] {
    while (running) {
      system(command.c_str());
      std::this_thread::sleep_for(std::chrono::milliseconds(Interval));
    }
  });

  std::cout << "Start validation\n";

  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < num_of_threads; i++) {
    threads.push_back(std::thread(ProcessTx));
  }

  for (int i = 0; i < num_of_threads; i++) {
    threads[i].join();
  }

  auto end = std::chrono::high_resolution_clock::now();

  if (validation_failure) {
    std::cerr << "Validation failed rate: " << (long double) validation_failure / num_of_blocks << std::endl;
  }

  running = false;
  vmstat_logger.join();

  std::chrono::duration<double> diff = end-start;
  std::cout << diff.count() << " sec\n";

  // output item list
  dump(path, "free");

  return 0;
}
