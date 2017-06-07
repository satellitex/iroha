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

#ifndef SERVICE_PROTOCOL_TO_STRING_HPP
#define SERVICE_PROTOCOL_TO_STRING_HPP

#include <flatbuffers/flatbuffers.h>
#include <main_generated.h>
#include <actions_generated.h>
#include <peer_actions_generated.h>
#include <asset_generated.h>
#include <domain_actions_generated.h>
#include <vector>

namespace protocol {

const std::string BROKEN_MESSAGE = "broken";

std::string to_string(const flatbuffers::Vector<uint8_t>* blob);
std::string to_string(const protocol::Signature* sig);
std::string to_string(const protocol::Status stat);
std::string to_string(const protocol::SignatureWithState* sig);
std::string to_string(const flatbuffers::Vector<
flatbuffers::Offset<protocol::SignatureWithState>>* sigs);
std::string to_string(const flatbuffers::Vector<
flatbuffers::Offset<protocol::Signature>>* sigs);
std::string to_string(uint64_t value);
std::string to_string(uint32_t value);
std::string to_string(uint16_t value);
std::string to_string(uint8_t value);
std::string to_string(double value);
std::string to_string(bool value);
std::string to_string(protocol::Action type);
std::string to_string(const flatbuffers::String* str);
std::string to_string(const protocol::AccountDelegate* act);
std::string to_string(const protocol::AccountUndelegate* act);
std::string to_string(const protocol::AccountSetQuorum* act);
std::string to_string(const protocol::SignatoriesRegister* act);
std::string to_string(const protocol::SignatoriesUnregister* act);
std::string to_string(const protocol::AccountActionWrapper* act);
std::string to_string(const protocol::Currency* currency);
std::string to_string(const protocol::ComplexAsset* complex);
std::string to_string(protocol::AnyAsset asset_type, const protocol::Asset* asset);
std::string to_string(const protocol::Asset* asset);
std::string to_string(const protocol::Add* act);
std::string to_string(const protocol::Subtract* act);
std::string to_string(const protocol::Transfer* act);
std::string to_string(const protocol::Exchange* act);
std::string to_string(const protocol::AssetSetPermission* act);
std::string to_string(const protocol::AssetActionWrapper* act);
std::string to_string(const protocol::AccountCreate* act);
std::string to_string(const protocol::AccountDelete* act);
std::string to_string(const protocol::AccountRegister* act);
std::string to_string(const protocol::AccountUnregister* act);
std::string to_string(const protocol::DomainCreate* act);
std::string to_string(const protocol::Delete* act);
std::string to_string(const protocol::DomainSetNotify* act);
std::string to_string(const protocol::DomainSetPermission* act);
std::string to_string(const protocol::Move* act);
std::string to_string(const protocol::AssetCreate* act);
std::string to_string(const protocol::DomainActionWrapper* act);
std::string to_string(const protocol::PeerAdd* act);
std::string to_string(const protocol::PeerRemove* act);
std::string to_string(const protocol::PeerChangeTrust* act);
std::string to_string(const protocol::PeerSetTrust* act);
std::string to_string(const protocol::PeerSetState* act);
std::string to_string(const protocol::PeerActionWrapper* act);
std::string to_string(const protocol::ActionWrapper* act);
std::string to_string(const flatbuffers::Vector<flatbuffers::Offset<
  protocol::ActionWrapper>>* acts);
std::string to_string(const protocol::Attachment* att);
std::string to_string(const protocol::Transaction* tx);

}  // namespace protocol

#endif
