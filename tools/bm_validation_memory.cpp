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
#include <flatbuffers/flatbuffers.h>
#include <utils/datetime.hpp>
#include <json.hpp>

#include <sys/stat.h>
#include <assert.h>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <thread>
#include <mutex>
#include <atomic>

struct Block {
  std::vector<uint8_t> tx;
  std::vector<uint8_t> signature;
};

std::vector<std::thread> threads;
std::mutex mtx_curr_tx;
std::mutex mtx_wsv;

std::vector<Block> blocks;
nlohmann::json wsv;

int num_of_blocks;
int num_of_threads;
int curr_tx;
int validation_failure;

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
  auto keyPair = signature::generateKeyPair();
  const auto signature = signature::sign(
    hash, keyPair.publicKey, keyPair.privateKey
  );
  return ::iroha::CreateSignatureDirect(
    fbb, base64::encode(keyPair.publicKey).c_str(), &signature, timestamp
  );
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

auto CreateBlock() {
  auto tx = CreateSampleTx(); // サイズは小さく作っても300bytes以上
  Block block;
  block.tx = tx;
  block.signature = CreateSignature(CreateTxHash(tx));
  return block;
}

void ProcessTx() {

  while (true) {
    mtx_curr_tx.lock();
    if (curr_tx >= num_of_blocks) {
      mtx_curr_tx.unlock();
      return;
    }
    auto &tx = blocks[curr_tx].tx;
    auto &sig = blocks[curr_tx].signature;
    curr_tx++;
    mtx_curr_tx.unlock();

    auto txHash = CreateTxHash(tx);
    auto sigroot = flatbuffers::GetRoot<::iroha::Signature>(sig.data());

    auto pk64 = sigroot->publicKey()->str();
    auto pkbytes = base64::decode(pk64);

    std::vector<uint8_t> sigbytes(
      sigroot->signature()->begin(),
      sigroot->signature()->end()
    );

    if (signature::verify(sigbytes, txHash, pkbytes)) {
      mtx_wsv.lock();
      wsv["signatories"].push_back(pk64);
      mtx_wsv.unlock();
    } else {
      validation_failure++;
    }
  }
}

void remove_if_exists(std::string const& path) {
  const std::string rm_cmd  = "rm " + path;
  struct stat st;
  if (stat(path.c_str(), &st) == 0) {
    std::cout << rm_cmd << std::endl;
    system(rm_cmd.c_str());
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

  auto items_path = path + "-" + item_name;
  remove_if_exists(items_path);

  std::cout << "output '" << item_name << "' CSV: " << items_path << std::endl;
  std::ofstream out(items_path);

  for (auto const& e: items) {
    out << std::get<0>(e) << "," << std::get<1>(e) << std::endl;
  }
}

void CreateBlocks(int num_of_blocks) {
  std::cout << "Create blocks...\n";
  for (int i = 0; i < num_of_blocks; i++) {
    blocks.push_back(CreateBlock());
  }
}

void CreateInitialWSV() {
  std::cout << "Create initial WSV (alternative json on memory)...\n";
  for (int i = 0; i < 1000; i++) {
    wsv["values"].push_back(random_string(20000));
  }
//  std::cout << wsv.dump(2) << std::endl;
}

int main(int argc, char** argv) {

  if (argc != 3) {
    std::cout << "Usage: ./bm_validation_memory [num of blocks] [num of threads]\n";
    exit(0);
  }

  num_of_blocks = std::stoi(std::string(argv[1]));
  num_of_threads = std::stoi(std::string(argv[2]));

  std::cout << "Size of tx = " << CreateBlock().tx.size() << std::endl;

  CreateBlocks(num_of_blocks);

  CreateInitialWSV();

  const std::string path = "/tmp/validation_memory";//-" + std::to_string((int)getpid());
  remove_if_exists(path);

  const std::string command = "vmstat >> " + path;

  constexpr int Interval = 1000;
  std::cout << "Interval: " << (double) Interval / 1000.0 << std::endl;
  std::cout << command << std::endl;

  // Outputs base vmstat
  system(command.c_str());

  std::cout << "Start validation...\n";

  std::atomic_bool vmstat_alive(true);
  std::thread vmstat_logger([&vmstat_alive, &command, Interval]{
    const std::chrono::milliseconds interval(Interval);
    while (vmstat_alive.load()) {
      auto start = std::chrono::system_clock::now();

      // Outputs vmstat while running.
      system(command.c_str());
      auto end = std::chrono::system_clock::now();
      auto wasted = end - start;
      if (wasted < interval){
        std::this_thread::sleep_for(interval - wasted);
      }
    }
  });

  auto start = std::chrono::system_clock::now();

  for (int i = 0; i < num_of_threads; i++) {
    threads.push_back(std::thread(ProcessTx));
  }

  for (int i = 0; i < num_of_threads; i++) {
    threads[i].join();
  }

  auto end = std::chrono::system_clock::now();

  if (validation_failure) {
    std::cerr << "Validation failed rate: " << (long double) validation_failure / num_of_blocks << std::endl;
  }

  vmstat_alive.store(false);
  vmstat_logger.join();

//  std::cout << wsv.dump(2) << std::endl;

  std::chrono::duration<double> diff = end-start;
  std::cout << diff.count() << " sec\n";

  std::cout << "output vmstat: " << path << "\n";

  // Outputs item list
  dump(path, "free");
  dump(path, "buff");
  dump(path, "cache");

  return 0;
}
