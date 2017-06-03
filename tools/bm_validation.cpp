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

#include <assert.h>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>

#include "bm_validation.hpp"

void process(Block const& block) {

  auto txHash = CreateTxHash(block.tx);
  auto sig = flatbuffers::GetRoot<::iroha::Signature>(block.signature.data());

  auto pk64 = sig->publicKey()->str();
  auto pkbytes = base64::decode(pk64);

  std::vector<uint8_t> sigbytes(
    sig->signature()->begin(),
    sig->signature()->end()
  );

  assert (signature::verify(sigbytes, txHash, pkbytes));
}

int main(int argc, char** argv) {

  std::vector<int> adds = {0, 100, 1000, 10000, (int)1e5, (int)1e6, (int)1e7, (int)1e8};
  std::vector<std::string> name_txsize = {"base", "+100", "+1000", "+10000", "+1e5", "+1e6", "+1e7", "+1e8"};

  if (argc < 2) {
    std::cout << "Usage: ./bm_validation try_times [type_count = 1]\n";// [additional]\n";
    exit(1);
  }

  const int try_times = std::stoi(std::string(argv[1]));
  int type_count = 1;

  if (argc > 2) {
    type_count = std::stoi(std::string(argv[2]));
    if (type_count < 1 || type_count > (int)adds.size()) {
      std::cout << "type_count's range: [1," << adds.size() << "]\n";
      exit(1);
    }
  }

  /*
   * 100万回電子署名をバリデーションして、回数と時間をCSVに出力する
   * トランザクションのサイズは複数指定できる
   */

  std::cout << "Calc validation duration (sec) " << try_times << " times.\n";

  const std::string path = "/tmp/validation_throughput.csv";
  remove_if_exists(path);
  std::ofstream out(path);
  out << "n-times,duration,txsize" << std::endl;

  for (int type_idx = 0; type_idx < type_count; type_idx++) {
    auto additional = adds[type_idx];
    const auto txsize = CreateBlock(additional).tx.size();
    std::cout << "Size of tx = " << txsize
                                 << (additional == 0 ? "(base txsize)\n" : "\n");
    for (int i = 0; i < try_times; i++) {
      auto block = CreateBlock(additional);
      auto start = std::chrono::system_clock::now();
      process(block);
      auto end = std::chrono::system_clock::now();

      std::chrono::duration<double> diff = end - start;
      out << i << "," << diff.count() << "," << name_txsize[type_idx] << std::endl;
    }
  }

  std::cout << "output: " << path << std::endl;

  return 0;
}
