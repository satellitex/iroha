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

#ifndef IROHA_RANDOM_TX_GENERATOR_HPP
#define IROHA_RANDOM_TX_GENERATOR_HPP

#include <flatbuffers/flatbuffers.h>
#include <main_generated.h>
#include <vector>
#include <random>
#include <cstdint>
#include <crypto/signature.hpp> // signature::KeyPair

namespace random_generator {

int32_t random_value_32(int32_t min, int32_t max);

// return random value of [min, max]
uint64_t random_value_64(uint64_t min, uint64_t max);

constexpr int MinQuorum = 1;
constexpr int MaxQuorum = 32;
constexpr size_t MaxStringSize = 1e9;

char random_alphabet();
char random_alnum();
std::string random_string(size_t length, std::function<char()> const& char_gen);
std::string random_alphabets(size_t length);
std::string random_alnums(size_t length);
std::vector<std::uint8_t> random_bytes(size_t length);
std::string random_sha3_256();
std::string random_base64();
std::string random_asset_id();
uint64_t random_time();
uint32_t random_nonce();

std::vector<uint8_t> random_signature(signature::KeyPair const& key_pair);

uint8_t random_quorum(int max = MaxQuorum);

flatbuffers::Offset<protocol::Signature> random_signature(
  flatbuffers::FlatBufferBuilder &fbb);

std::vector<flatbuffers::Offset<protocol::Signature>> random_signatures(
  flatbuffers::FlatBufferBuilder &fbb, int length = 10);

flatbuffers::Offset<protocol::Attachment> random_attachment(
  flatbuffers::FlatBufferBuilder &fbb);

flatbuffers::Offset<protocol::ActionWrapper> random_AccountCreate(
  flatbuffers::FlatBufferBuilder &fbb,
  std::string const username = random_alphabets(15));

flatbuffers::Offset<protocol::ComplexAsset> random_ComplexAsset(
  flatbuffers::FlatBufferBuilder &fbb);

flatbuffers::Offset<protocol::Asset> random_AssetCurrency(
  flatbuffers::FlatBufferBuilder &fbb);

flatbuffers::Offset<protocol::Asset> random_AssetComplexAsset(
  flatbuffers::FlatBufferBuilder &fbb);

flatbuffers::Offset<protocol::Asset> random_Asset(
  flatbuffers::FlatBufferBuilder &fbb,
  protocol::AnyAsset anyAsset);

flatbuffers::Offset<protocol::Asset> random_Asset(
  flatbuffers::FlatBufferBuilder &fbb);

flatbuffers::Offset<protocol::ActionWrapper> random_AssetCreate(
  flatbuffers::FlatBufferBuilder &fbb);

std::vector<uint8_t> random_tx(
  flatbuffers::FlatBufferBuilder &fbb,
  std::vector<flatbuffers::Offset<protocol::ActionWrapper>> actions,
  flatbuffers::Offset<protocol::Attachment> attachment);

}  // namespace generator

namespace dump {

std::string toString(const flatbuffers::Vector<uint8_t>& blob);

std::string tab(int indent);

std::string toString(protocol::Signature const& signature,
                     std::string const& name, int indent = 0);

std::string toString(flatbuffers::Vector<
                     flatbuffers::Offset<protocol::Signature>> const& sigs,
                     std::string const& name, int indent = 0);

std::string toString(const protocol::Transaction& tx);

}  // namespace dump

#endif //IROHA_RANDOM_GENERATOR_HPP
