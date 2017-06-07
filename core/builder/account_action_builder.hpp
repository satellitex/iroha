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

#ifndef IROHA_ACCOUNT_ACTION_BUILDER_HPP
#define IROHA_ACCOUNT_ACTION_BUILDER_HPP

#include <account_actions_generated.h>
#include <primitives_generated.h>
#include <map>
#include <vector>

namespace builder{

    using Timestamp = unsigned long;
    using ubyte     = unsigned char;
    using Signature   = protocol::Signature;
    using AccountAction = protocol::AccountAction;

    class AccountActionBuilder{

        std::string                  to_account;       // AccountAddAccount
        std::string                  from_account;     // SignatoriesUnregister
        std::string                  target_account;   // AccountSetQuorum
        std::string                  auditor_account;  // AccountDelegate / AccountUndelegate
        ubyte                        quorum;           // AccountSetQuorum
        std::vector<SignatureHolder> signatories;      // SignatoriesRegister / SignatoriesRegister

        enum class State{
            UnderConstruction,
            AfterConstruction
        } state;

        AccountAction action;
        static std::map<
                AccountAction,
                std::function<flatbuffers::Offset<void>(
                        flatbuffers::FlatBufferBuilder&,
                        const AccountActionBuilder&
                )>> buildFunctions;

    public:

        AccountActionBuilder();

        AccountActionBuilder& setToAccount(const std::string&);       // AccountAddAccount
        AccountActionBuilder& setFromAccount(const std::string&);     // SignatoriesUnregister
        AccountActionBuilder& setTargetAccount(const std::string&);   // AccountSetQuorum
        AccountActionBuilder& setAuditorAccount(const std::string&);  // AccountDelegate / AccountUndelegate
        AccountActionBuilder& setQuorum(ubyte);                       // AccountSetQuorum
        AccountActionBuilder& setSignatories(const std::vector<Signature>&); // SignatoriesRegister / SignatoriesRegister
        AccountActionBuilder& addSignatories(const  Signature& _signature);

        AccountActionBuilder& as(const AccountAction&);
        flatbuffers::Offset<void> buildOffset(flatbuffers::FlatBufferBuilder& fbb);
        flatbuffers::Offset<protocol::AccountActionWrapper> buildWrapperOffset(flatbuffers::FlatBufferBuilder& fbb);
        flatbuffers::Offset<void> buildWrapperUnionOffset(flatbuffers::FlatBufferBuilder& fbb);

    };
};

#endif

