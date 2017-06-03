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
#include <map>

#include "bm_validation.hpp"

std::vector<std::thread> threads;
std::mutex mtx_curr_tx;
std::mutex mtx_wsv;

std::vector<Block> blocks;
nlohmann::json wsv;

int num_of_blocks;
int num_of_threads;
int curr_tx;
int validation_failure;

void process() {

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

void output_csv(std::string const& path,
                std::vector<std::string> const& item_names) {

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

  std::map<int, std::map<std::string, int64_t>> items;

  int curr = 0;

  for (size_t i = 1; i < buf.size(); i += 3) {
    for (size_t j = 0; j < buf[i].size(); j++) {
      for (auto const& item_name: item_names) {
        if (buf[i][j] == item_name) {
          assert(i + 1 < buf.size());
          items[i / 3][item_name] = std::stoll(buf[i + 1][j]);
        }
      }
    }
  }

  auto items_path = path + ".csv";
  remove_if_exists(items_path);

  std::cout << "output CSV: " << items_path << std::endl;
  std::ofstream out(items_path);

  out << "time";
  for (auto const& item: item_names)
    out << "," << item;
  out << std::endl;

  for (auto& row: items) {
    auto index = row.first;
    auto& value = row.second;
    out << index;
    for (auto const& item_name: item_names) {
      out << "," << value[item_name];
    }
    out << std::endl;
  }
}

void create_blocks(int num_of_blocks) {
  std::cout << "Create blocks...\n";
  for (int i = 0; i < num_of_blocks; i++) {
    blocks.push_back(CreateBlock(0));
  }
}

void create_init_wsv() {
  std::cout << "Create initial WSV (alternative json on memory)...\n";
  for (int i = 0; i < 1000; i++) {
    wsv["values"].push_back(random_string(20000));
  }
//  std::cout << wsv.dump(2) << std::endl;
}

int main(int argc, char** argv) {

  if (argc != 3) {
    std::cout << "Usage: ./bm_validation_memory num_of_blocks num_of_threads\n";
    exit(1);
  }

  num_of_blocks = std::stoi(std::string(argv[1]));
  num_of_threads = std::stoi(std::string(argv[2]));

  std::cout << "Size of tx = " << CreateBlock(0).tx.size() << std::endl;

  create_blocks(num_of_blocks);
  create_init_wsv();

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

    // prevent from recording at 0 sec.
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();

    while (vmstat_alive.load()) {
      auto wasted = end - start;
      if (wasted < interval) {
        std::this_thread::sleep_for(interval - wasted);
      }

      start = std::chrono::system_clock::now();
      // Outputs vmstat while running.
      system(command.c_str());
      end = std::chrono::system_clock::now();
    }
  });

  auto start = std::chrono::system_clock::now();

  for (int i = 0; i < num_of_threads; i++) {
    threads.push_back(std::thread(process));
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
  output_csv(path, {"free", "buff", "cache"});

  return 0;
}
