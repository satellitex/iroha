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
#include <main_generated.h>
#include <actions_generated.h>
#include <asset_generated.h>
#include <domain_actions_generated.h>
#include <vector>
#include <random>
#include <crypto/hash.hpp> // sha3_256
#include <crypto/base64.hpp> // encode
#include <crypto/signature.hpp> // sign

#include "random_generator.hpp"

namespace random_generator {

std::random_device seed_gen;
std::mt19937    rnd_gen_32(seed_gen());
std::mt19937_64 rnd_gen_64(seed_gen());

// return random value of [min, max]
int32_t random_value_32(int32_t min, int32_t max)
{
  assert(min <= max);
  std::uniform_int_distribution<int32_t> dist(min, max);
  return dist(rnd_gen_32);
}

// return random value of [min, max]
uint64_t random_value_64(uint64_t min, uint64_t max)
{
  assert(min <= max);
  std::uniform_int_distribution<uint64_t> dist(min, max);
  return dist(rnd_gen_64);
}

char random_alphabet()
{
  const std::string buf = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  const int size = buf.size();
  return buf[random_value_32(0, size - 1)];
}

char random_alnum()
{
  const std::string buf = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  const int size = buf.size();
  return buf[random_value_32(0, size - 1)];
}

std::string random_string(size_t length, std::function<char()> const& char_gen)
{
  std::string ret;
  for (size_t i = 0; i < length; i++) {
    ret += char_gen();
  }
  return ret;
}

std::string random_alphabets(size_t length)
{
  assert(length <= MaxStringSize);
  return random_string(length, random_alphabet);
}

std::string random_alnums(size_t length)
{
  return random_string(length, random_alnum);
}

std::vector<std::uint8_t> random_bytes(size_t length)
{
  std::vector<uint8_t> ret;
  for (size_t i = 0; i < length; i++) {
    ret.push_back((uint8_t)random_value_32(0, (1 << 8) - 1));
  }
  return ret;
}

std::string random_sha3_256()
{
  auto raw_value = random_alphabets((size_t)random_value_32(1, 50));
  return hash::sha3_256_hex(raw_value);
}

std::string random_base64()
{
  auto raw_value = random_alphabets((size_t)random_value_32(1, 50));
  std::vector<unsigned char> value;
  for (auto e: raw_value) {
    value.push_back(e);
  }
  return base64::encode(value);
}

std::string random_full_domain_text(size_t depth = 3, size_t token_max_length = 8)
{
  std::string ret;
  for (size_t i = 0; i < depth; i++) {
    if (i) ret += ".";
    ret += random_alnums(token_max_length);
  }
  return ret;
}

std::string random_account_id()
{
  return random_alnums(10) + "@" + random_full_domain_text();
}

std::string random_asset_id()
{
  return random_alnums(10) + "#" + random_full_domain_text();
}

std::string random_target_domain(size_t depth = 3)
{
  return random_full_domain_text(depth);
}

uint64_t random_time()
{
  return random_value_64(0ULL, 1ULL << 63);
}

uint32_t random_nonce()
{
  return (uint32_t)random_value_32(0, 1 << 30);
}

std::vector<uint8_t> random_signature(signature::KeyPair const& key_pair)
{
  auto message = random_alphabets(50);
  auto res_str = signature::sign(message, key_pair);
  std::vector<uint8_t> res;
  for (auto e: res_str)
    res.push_back((unsigned char)e);
  return res;
}

uint8_t random_quorum(int max) {
  assert(MinQuorum <= max);
  return (uint8_t)random_value_32(MinQuorum, max);
}

flatbuffers::Offset<protocol::Signature> random_signature(
  flatbuffers::FlatBufferBuilder &fbb)
{
  auto key_pair = signature::generateKeyPair();
  auto pubkey = base64::encode(key_pair.publicKey);
  auto pubkey_v = std::vector<uint8_t>(pubkey.begin(), pubkey.end());

  return protocol::CreateSignature(
    fbb,
    fbb.CreateVector(pubkey_v),
    fbb.CreateVector(random_signature(key_pair))
  );
}

flatbuffers::Offset<protocol::SignatureWithState> random_signature_with_state(
  flatbuffers::FlatBufferBuilder &fbb)
{
  auto key_pair = signature::generateKeyPair();
  return protocol::CreateSignatureWithState(
    fbb, random_signature(fbb), protocol::Status::Approve
  );
}

std::vector<flatbuffers::Offset<protocol::Signature>> random_signatures(
  flatbuffers::FlatBufferBuilder &fbb, int length)
{
  std::vector<flatbuffers::Offset<protocol::Signature>> ret;
  for (int i = 0; i < length; i++) {
    ret.push_back(random_signature(fbb));
  }
  return ret;
}

std::vector<flatbuffers::Offset<protocol::SignatureWithState>> random_signature_with_states(
  flatbuffers::FlatBufferBuilder &fbb, int length)
{
  std::vector<flatbuffers::Offset<protocol::SignatureWithState>> ret;
  for (int i = 0; i < length; i++) {
    ret.push_back(random_signature_with_state(fbb));
  }
  return ret;
}

flatbuffers::Offset<protocol::Attachment> random_attachment(
  flatbuffers::FlatBufferBuilder &fbb)
{
  return protocol::CreateAttachment(
    fbb, fbb.CreateString(random_alphabets(50)),
    fbb.CreateVector(random_bytes(50)));
}

flatbuffers::Offset<protocol::ActionWrapper> random_AccountCreate(
  flatbuffers::FlatBufferBuilder &fbb,
  std::string const username)
{
  auto act = protocol::CreateAccountCreate(
    fbb, fbb.CreateString(username), fbb.CreateVector(random_signatures(fbb, 5)),
    random_quorum(3), fbb.CreateString(random_target_domain())
  );
  auto dactw = protocol::CreateDomainActionWrapper(
    fbb, protocol::DomainAction::AccountCreate, act.Union()
  );
  return protocol::CreateActionWrapper(
    fbb, protocol::Action::DomainActionWrapper, dactw.Union()
  );
}

flatbuffers::Offset<protocol::Currency> random_Curerncy(
  flatbuffers::FlatBufferBuilder &fbb,
  size_t num_of_digits = 15)
{
  std::vector<uint8_t> amount;
  if (num_of_digits <= 0) {
    throw std::runtime_error("num_of_digits should be positive");
  }
  for (size_t i = 0; i < num_of_digits; i++) {
    amount.push_back((uint8_t)random_value_32(0, 9));
  }
  return protocol::CreateCurrency(
    fbb, fbb.CreateVector(amount), (uint8_t)random_value_32(0, (uint8_t)num_of_digits - 1)
  );
}

flatbuffers::Offset<protocol::ActionWrapper> random_AssetCreate(
  flatbuffers::FlatBufferBuilder &fbb,
  protocol::AnyAsset asset_type, flatbuffers::Offset<void> asset_content)
{
  auto asset = protocol::CreateAsset(
    fbb, fbb.CreateString(random_alnums(10)/*random_asset_id()*/),
    fbb.CreateString(random_alnums(32)), fbb.CreateString(random_full_domain_text()),
    asset_type, asset_content
  );

  auto asset_create = protocol::CreateAssetCreate(fbb, asset);
  auto domain_action = protocol::CreateDomainActionWrapper(
    fbb, protocol::DomainAction::AssetCreate, asset_create.Union()
  );
  return protocol::CreateActionWrapper(
    fbb, protocol::Action::DomainActionWrapper, domain_action.Union()
  );
}

std::vector<uint8_t> random_tx(
  flatbuffers::FlatBufferBuilder &fbb,
  std::vector<flatbuffers::Offset<protocol::ActionWrapper>> actions,
  flatbuffers::Offset<protocol::Attachment> attachment)
{
  auto tx = protocol::CreateTransaction(
    fbb,
    random_signature(fbb),
    fbb.CreateVector(random_signature_with_states(fbb, 3)),
    random_time(),
    random_nonce(),
    fbb.CreateVector(actions),
    attachment
  );

  fbb.Finish(tx);

  uint8_t *ptr = fbb.GetBufferPointer();
  return {ptr, ptr + fbb.GetSize()};
}

}  // namespace generator
