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

#include "protocol_to_string.hpp"

namespace protocol {

std::string to_string(const flatbuffers::Vector<uint8_t>* blob)
{
  std::string ret;
  for (auto e: *blob) {
    if (e == '\0') ret += "\\0";
    else ret += e;
  }
  return ret;
}

std::string to_string(const protocol::Signature* sig) {
  return to_string(sig->pubkey()) +
         to_string(sig->sig());
}

std::string to_string(const protocol::Status stat) {
  return std::to_string(static_cast<int>(stat));
}

std::string to_string(const protocol::SignatureWithState* sig) {
  return to_string(sig->signature()) +
         to_string(sig->state());
}

std::string to_string(const flatbuffers::Vector<
flatbuffers::Offset<protocol::SignatureWithState>>* sigs) {
  std::string ret;
  for (auto sig: *sigs) {
    ret += to_string(sig);
  }
  return ret;
}

std::string to_string(const flatbuffers::Vector<
flatbuffers::Offset<protocol::Signature>>* sigs) {
  std::string ret;
  for (auto sig: *sigs) {
    ret += to_string(sig);
  }
  return ret;
}

std::string to_string(uint64_t value) {
  return std::to_string(value);
}

std::string to_string(uint32_t value) {
  return std::to_string(value);
}

std::string to_string(uint16_t value) {
  return std::to_string(value);
}

std::string to_string(uint8_t value) {
  return std::to_string(value);
}

std::string to_string(double value) {
  return std::to_string(value);
}

std::string to_string(bool value) {
  return std::to_string(value);
}

std::string to_string(protocol::Action type) {
  return protocol::EnumNameAction(type);
}

std::string to_string(const flatbuffers::String* str) {
  return str == nullptr ? "" : str->str();
}

std::string to_string(const protocol::AccountDelegate* act) {
  return to_string(act->auditor_account());
}

std::string to_string(const protocol::AccountUndelegate* act) {
  return to_string(act->auditor_account());
}

std::string to_string(const protocol::AccountSetQuorum* act) {
  return to_string(act->target_account()) +
         to_string(act->quorum());
}

std::string to_string(const protocol::SignatoriesRegister* act) {
  return to_string(act->to_account()) +
         to_string(act->signatories());
}

std::string to_string(const protocol::SignatoriesUnregister* act) {
  return to_string(act->from_account()) +
         to_string(act->signatories());
}

std::string to_string(const protocol::AccountActionWrapper* act) {
  switch (act->action_type()) {
    case protocol::AccountAction::AccountDelegate: {
      return to_string(act->action_as_AccountDelegate());
    }
    case protocol::AccountAction::AccountUndelegate: {
      return to_string(act->action_as_AccountUndelegate());
    }
    case protocol::AccountAction::AccountSetQuorum: {
      return to_string(act->action_as_AccountSetQuorum());
    }
    case protocol::AccountAction::SignatoriesRegister: {
      return to_string(act->action_as_SignatoriesRegister());
    }
    case protocol::AccountAction::SignatoriesUnregister: {
      return to_string(act->action_as_SignatoriesUnregister());
    }
    default: {
      return BROKEN_MESSAGE;
    }
  }
}

std::string to_string(const protocol::Currency* currency) {
  return to_string(currency->amount()) +
         to_string(currency->precision());
}

std::string to_string(const protocol::ComplexAsset* complex) {
  return to_string(complex->prop());
}

std::string to_string(protocol::AnyAsset asset_type, const protocol::Asset* asset) {
  switch (asset_type) {
    case protocol::AnyAsset::Currency: {
      return to_string(asset->asset_as_Currency());
    }
    case protocol::AnyAsset::ComplexAsset: {
      return to_string(asset->asset_as_ComplexAsset());
    }
    default: {
      return BROKEN_MESSAGE;
    }
  }
}

std::string to_string(const protocol::Asset* asset) {
  return to_string(asset->name()) +
         to_string(asset->uuid()) +
         to_string(asset->target_domain()) +
         to_string(asset->asset_type(), asset);
}

std::string to_string(const protocol::Add* act) {
  return to_string(act->target_account()) +
         to_string(act->asset());
}

std::string to_string(const protocol::Subtract* act) {
  return to_string(act->target_account()) +
         to_string(act->asset());
}

std::string to_string(const protocol::Transfer* act) {
  return to_string(act->sender_account()) +
         to_string(act->receiver_account()) +
         to_string(act->asset());
}

std::string to_string(const protocol::Exchange* act) {
  return to_string(act->sender_account()) +
         to_string(act->sender_asset()) +
         to_string(act->receiver_account()) +
         to_string(act->receiver_asset());
}

std::string to_string(const protocol::AssetSetPermission* act) {
  return to_string(act->account()) +
         to_string(act->asset_uuid()) +
         to_string(act->permission());
}

std::string to_string(const protocol::AssetActionWrapper* act) {
  switch (act->action_type()) {
    case protocol::AssetAction::Add: {
      return to_string(act->action_as_Add());
    }
    case protocol::AssetAction::Subtract: {
      return to_string(act->action_as_Subtract());
    }
    case protocol::AssetAction::Transfer: {
      return to_string(act->action_as_Transfer());
    }
    case protocol::AssetAction::Exchange: {
      return to_string(act->action_as_Exchange());
    }
    case protocol::AssetAction::AssetSetPermission: {
      return to_string(act->action_as_AssetSetPermission());
    }
    default: {
      return BROKEN_MESSAGE;
    }
  }
}

std::string to_string(const protocol::AccountCreate* act) {
  return to_string(act->username()) +
         to_string(act->target_domain()) +
         to_string(act->signatories()) +
         to_string(act->quorum());
}

std::string to_string(const protocol::AccountDelete* act) {
  return to_string(act->target_account());
}

std::string to_string(const protocol::AccountRegister* act) {
  return to_string(act->permission()) +
         to_string(act->target_account()) +
         to_string(act->target_domain());
}

std::string to_string(const protocol::AccountUnregister* act) {
  return to_string(act->target_account()) +
         to_string(act->target_domain());
}

std::string to_string(const protocol::DomainCreate* act) {
  return to_string(act->domain()) +
         to_string(act->root_account());
}

std::string to_string(const protocol::Delete* act) {
  return to_string(act->target());
}

std::string to_string(const protocol::DomainSetNotify* act) {
  return to_string(act->is_notify()) +
         to_string(act->target_domain());
}

std::string to_string(const protocol::DomainSetPermission* act) {
  return to_string(act->domain()) +
         to_string(act->permission()) +
         to_string(act->target_account());
}

std::string to_string(const protocol::Move* act) {
  return to_string(act->source()) +
         to_string(act->destination());
}

std::string to_string(const protocol::AssetCreate* act) {
  return to_string(act->asset());
}

std::string to_string(const protocol::DomainActionWrapper* act) {
  switch (act->action_type()) {
    case protocol::DomainAction::AccountCreate: {
      return to_string(act->action_as_AccountCreate());
    }
    case protocol::DomainAction::AccountDelete: {
      return to_string(act->action_as_AccountDelete());
    }
    case protocol::DomainAction::AccountRegister: {
      return to_string(act->action_as_AccountRegister());
    }
    case protocol::DomainAction::AccountUnregister: {
      return to_string(act->action_as_AccountUnregister());
    }
    case protocol::DomainAction::DomainCreate: {
      return to_string(act->action_as_DomainCreate());
    }
    case protocol::DomainAction::Delete: {
      return to_string(act->action_as_Delete());
    }
    case protocol::DomainAction::DomainSetNotify: {
      return to_string(act->action_as_DomainSetNotify());
    }
    case protocol::DomainAction::DomainSetPermission: {
      return to_string(act->action_as_DomainSetPermission());
    }
    case protocol::DomainAction::Move: {
      return to_string(act->action_as_Move());
    }
    case protocol::DomainAction::AssetCreate: {
      return to_string(act->action_as_AssetCreate());
    }
    default: {
      return BROKEN_MESSAGE;
    }
  }
}

std::string to_string(const protocol::PeerAdd* act) {
  return to_string(act->peer());
}

std::string to_string(const protocol::PeerRemove* act) {
  return to_string(act->target_peer());
}

std::string to_string(const protocol::PeerChangeTrust* act) {
  return to_string(act->target_peer()) +
         to_string(act->trust());
}

std::string to_string(const protocol::PeerSetTrust* act) {
  return to_string(act->target_peer()) +
         to_string(act->trust());
}

std::string to_string(const protocol::PeerSetState* act) {
  return to_string(act->target_peer()) +
         to_string(act->state());
}

std::string to_string(const protocol::PeerActionWrapper* act) {
  switch (act->action_type()) {
    case protocol::PeerAction::PeerAdd: {
      return to_string(act->action_as_PeerAdd());
    }
    case protocol::PeerAction::PeerRemove: {
      return to_string(act->action_as_PeerRemove());
    }
    case protocol::PeerAction::PeerChangeTrust: {
      return to_string(act->action_as_PeerChangeTrust());
    }
    case protocol::PeerAction::PeerSetTrust: {
      return to_string(act->action_as_PeerSetState());
    }
    case protocol::PeerAction::PeerSetState: {
      return to_string(act->action_as_PeerSetState());
    }
  }
}

std::string to_string(const protocol::ActionWrapper* act) {
  switch (act->action_type()) {
    case protocol::Action::AccountActionWrapper: {
      return to_string(act->action_as_AccountActionWrapper());
    }
    case protocol::Action::AssetActionWrapper: {
      return to_string(act->action_as_AssetActionWrapper());
    }
    case protocol::Action::DomainActionWrapper: {
      return to_string(act->action_as_DomainActionWrapper());
    }
    case protocol::Action::PeerActionWrapper: {
      return to_string(act->action_as_PeerActionWrapper());
    }
    default: {
      return BROKEN_MESSAGE;
    }
  }
}

std::string to_string(const flatbuffers::Vector<flatbuffers::Offset<
  protocol::ActionWrapper>>* acts) {
  std::string ret;
  for (auto e: *acts) {
    ret += to_string(e);
  }
  return ret;
}

std::string to_string(const protocol::Attachment* att) {
  return to_string(att->mime()) +
         to_string(att->data());
}

std::string to_string(const protocol::Transaction* tx) {
  return to_string(tx->creator()) +
         to_string(tx->sigs()) +
         to_string(tx->created()) +
         to_string(tx->nonce()) +
         to_string(tx->actions()) +
         to_string(tx->attachment());
}

}  // namespace protocol

