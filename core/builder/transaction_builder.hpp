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
#ifndef IROHA_TRANSACTION_BUILDER_HPP
#define IROHA_TRANSACTION_BUILDER_HPP

#include <transaction_generated.h>
#include <primitives_generated.h>

#include "primitive_builder.hpp"
#include <vector>

namespace builder{

    using Timestamp = unsigned long;
    using ubyte     = unsigned char;
    using Signature   = protocol::Signature;
    using Transaction = protocol::Transaction;

    class TransactionBuilder{

        SignatureHolder              creator;
        std::vector<SignatureHolder> sigs;
        Timestamp                    created;
        std::vector<uint32_t>        nonce;
        flatbuffers::Offset<void>    action;
        protocol::Action             action_type;

        enum class State{
            UnderConstruction,
            AfterConstruction
        } state;

    public:
        TransactionBuilder();

        TransactionBuilder& setCreator(const Signature&);
        TransactionBuilder& setSigs(const    std::vector<Signature>&&);
        TransactionBuilder& addSig(const     Signature&&);

        TransactionBuilder& setCreated(const Timestamp&);
        TransactionBuilder& setNonce(const   std::vector<uint32_t>&);
        TransactionBuilder& setAction(const protocol::Action&,const  flatbuffers::Offset<void>&);

        flatbuffers::Offset<Transaction> buildOffset(flatbuffers::FlatBufferBuilder& fbb);
        flatbuffers::BufferRef<Transaction> buildBufferRef(flatbuffers::FlatBufferBuilder& fbb);
    };

};

#endif