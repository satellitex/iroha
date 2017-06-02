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

  if (argc < 2) {
    std::cout << "Usage: ./bm_validation try_times [additional]\n";
    exit(0);
  }

  const int try_times = std::stoi(std::string(argv[1]));

  int additional = 0;
  if (argc > 2) {
    if (additional < 0) {
      std::cout << "'additional' should not be negative.\n";
      exit(0);
    }
    additional = std::stoi(std::string(argv[2]));
  }

  /*
   * 100万回電子署名をバリデーションして、回数と時間をCSVに出力する
   */

  std::cout << "Size of tx = " << CreateBlock(additional).tx.size() << std::endl;

  std::cout << "Calc validation duration (sec) " << try_times << " times.\n";

  std::ofstream out("/tmp/validation_throughput.csv");
  out << "n-times,duration" << std::endl;

  for (int i = 0; i < try_times; i++) {
    auto block = CreateBlock(additional);
    auto start = std::chrono::system_clock::now();
    process(block);
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> diff = end-start;
    out << i << "," << diff.count() << std::endl;
  }

  return 0;
}
