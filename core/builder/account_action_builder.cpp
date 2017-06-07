/*
Copyright Soramitsu Co., Ltd. 2016 All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

                 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "primitive_builder.hpp"
#include "build_validator.hpp"
#include "account_action_builder.hpp"

namespace builder{

    using Timestamp = unsigned long;
    using ubyte     = unsigned char;
    using Signature   = protocol::Signature;
    using AccountAction = protocol::AccountAction;
    using ActionBuilder = AccountActionBuilder;

    AccountActionBuilder::AccountActionBuilder():
            action(AccountAction::NONE),
            quorum(0)
    {}


    ActionBuilder& ActionBuilder::setToAccount(const std::string& _to_account){
        this->to_account = _to_account;
        return *this;
    }

    ActionBuilder& ActionBuilder::setFromAccount(const std::string& _from_account){
        this->from_account = _from_account;
        return *this;
    }

    ActionBuilder& ActionBuilder::setTargetAccount(const std::string& _target_account){
        this->target_account = _target_account;
        return *this;
    }

    ActionBuilder& ActionBuilder::setAuditorAccount(const std::string& _auditor_account){
        this->auditor_account = _auditor_account;
        return *this;
    }

    ActionBuilder& ActionBuilder::setQuorum(ubyte _quorum){
        this->quorum = _quorum;
        return *this;
    }

    ActionBuilder& ActionBuilder::setSignatories(const  std::vector<Signature>& _signatures){
        for(auto& sig: _signatures){
            signatories.push_back(
                    SignatureHolder(sig)
            );
        }
        return *this;
    }

    ActionBuilder& ActionBuilder::addSignatories(const  Signature& _signature){
        signatories.push_back(
                SignatureHolder(_signature)
        );
        return *this;
    }

    ActionBuilder& ActionBuilder::as(const AccountAction& _action){
        this->action = _action;
        return *this;
    }

    flatbuffers::Offset<void> ActionBuilder::buildOffset(
            flatbuffers::FlatBufferBuilder& fbb
    ){
        if(ActionBuilder::buildFunctions.find(this->action) == ActionBuilder::buildFunctions.end()){
            if(this->action == AccountAction::NONE){
                throw std::runtime_error("Error! you should call as(ActionAction)");
            }
            throw std::runtime_error("Error! " + std::string(EnumNameAccountAction(this->action)) + " is not implemented");
        }
        return ActionBuilder::buildFunctions[this->action](fbb,*this);
    }

    flatbuffers::Offset<protocol::AccountActionWrapper> ActionBuilder::buildWrapperOffset(
            flatbuffers::FlatBufferBuilder& fbb
    ){
        return CreateAccountActionWrapper(fbb,
                                          this->action,
                                          this->buildOffset(fbb).Union()
        );
    }

    flatbuffers::Offset<void> ActionBuilder::buildWrapperUnionOffset(flatbuffers::FlatBufferBuilder& fbb){
        return this->buildWrapperOffset(fbb).Union();
    }

    std::map<AccountAction,std::function<flatbuffers::Offset<void>(
            flatbuffers::FlatBufferBuilder& fbb,
            const ActionBuilder&
    )>> ActionBuilder::buildFunctions = {

            { AccountAction::SignatoriesRegister, [&](flatbuffers::FlatBufferBuilder& fbb, const ActionBuilder& builder) -> flatbuffers::Offset<void>{
                // === Require validator ===
                isSufficeStr(builder.to_account,"to_account");
                isSufficeSignatureHolder(builder.signatories,"signatories");
                // =========================
                std::vector<flatbuffers::Offset<protocol::Signature>> sigs_vec;
                for(auto& sig: builder.signatories){
                    sigs_vec.push_back(SignatureBuilder(sig).buildOffset(fbb));
                }
                return CreateSignatoriesRegisterDirect(
                        fbb,
                        builder.to_account.c_str(),
                        &sigs_vec
                ).Union();
            }},
            { AccountAction::SignatoriesUnregister, [&](flatbuffers::FlatBufferBuilder& fbb, const ActionBuilder& builder) -> flatbuffers::Offset<void>{
                // === Require validator ===
                isSufficeStr(builder.from_account,"from_account");
                isSufficeSignatureHolder(builder.signatories,"signatories");
                // =========================
                std::vector<flatbuffers::Offset<protocol::Signature>> sigs_vec;
                for(auto& sig: builder.signatories){
                    sigs_vec.push_back(SignatureBuilder(sig).buildOffset(fbb));
                }
                return CreateSignatoriesUnregisterDirect(
                        fbb,
                        builder.from_account.c_str(),
                        &sigs_vec
                ).Union();
            }},
            { AccountAction::AccountSetQuorum, [&](flatbuffers::FlatBufferBuilder& fbb, const ActionBuilder& builder) -> flatbuffers::Offset<void>{
                // === Require validator ===
                isSufficeStr(builder.target_account,"target_account");
                // =========================
                return protocol::CreateAccountSetQuorumDirect(
                        fbb,
                        builder.target_account.c_str(),
                        builder.quorum
                ).Union();
            }},
            { AccountAction::AccountDelegate, [&](flatbuffers::FlatBufferBuilder& fbb, const ActionBuilder& builder) -> flatbuffers::Offset<void>{
                // === Require validator ===
                isSufficeStr(builder.auditor_account,"auditor_account");
                // =========================
                return protocol::CreateAccountDelegateDirect(
                        fbb,
                        builder.auditor_account.c_str()
                ).Union();
            }},
            { AccountAction::AccountUndelegate, [&](flatbuffers::FlatBufferBuilder& fbb, const ActionBuilder& builder) -> flatbuffers::Offset<void>{
                // === Require validator ===
                isSufficeStr(builder.auditor_account,"auditor_account");
                // =========================
                return protocol::CreateAccountUndelegateDirect(
                        fbb,
                        builder.auditor_account.c_str()
                ).Union();
            }}
    };

};