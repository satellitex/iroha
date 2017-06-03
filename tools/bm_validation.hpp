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

#include <flatbuffers/flatbuffers.h>
#include <service/flatbuffer_service.h>
#include <crypto/signature.hpp>
#include <crypto/hash.hpp>
#include <crypto/base64.hpp>
#include <random>
#include <sys/stat.h>

#ifndef BM_VALIDATION_HPP
#define BM_VALIDATION_HPP

std::random_device seed_gen;
std::mt19937    rnd_gen_32(seed_gen());
std::mt19937_64 rnd_gen_64(seed_gen());

// #include <utils/random.hpp>
// return random value of [min, max]
inline int32_t random_value_32(int32_t min, int32_t max) {
  assert(min <= max);
  std::uniform_int_distribution<int32_t> dist(min, max);
  return dist(rnd_gen_32);
}

// return random value of [min, max]
inline uint64_t random_value_64(uint64_t min, uint64_t max) {
  assert(min <= max);
  std::uniform_int_distribution<uint64_t> dist(min, max);
  return dist(rnd_gen_64);
}

std::string random_string(int length) {
  std::string ret;
  for (int i=0; i<length; i++) {
    ret += 'a' + random_value_32(0, 25);
  }
  return ret;
}

std::vector<uint8_t> CreateSampleTx(int att_str_size = 0) {
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
    std::vector<uint8_t> data;
    return ::iroha::CreateAttachmentDirect(
      fbb, random_string(att_str_size).c_str(), &data);
  }();

  const auto txOffset = ::iroha::CreateTransactionDirect(
    fbb, "pk", iroha::Command::AccountAdd,
    ::iroha::CreateAccountAddDirect(fbb, &accountBuf).Union(),
    &signatureOffsets, &_hash, 10000, attachmentOffset);

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

struct Block {
  std::vector<uint8_t> tx;
  std::vector<uint8_t> signature;
};

auto CreateBlock(int additinal) {
  auto tx = CreateSampleTx(additinal); // サイズは小さく作っても300bytes以上
  Block block;
  block.tx = tx;
  block.signature = CreateSignature(CreateTxHash(tx));
  return block;
}

void remove_if_exists(std::string const& path) {
  const std::string rm_cmd  = "rm " + path;
  struct stat st;
  if (stat(path.c_str(), &st) == 0) {
    std::cout << rm_cmd << std::endl;
    system(rm_cmd.c_str());
  }
}

#endif
