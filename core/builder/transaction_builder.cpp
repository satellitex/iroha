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

#include <iostream>
#include "transaction_builder.hpp"

namespace builder{

    TransactionBuilder::TransactionBuilder()
            :state(State::UnderConstruction)
    {}

    TransactionBuilder& TransactionBuilder::setCreator(const Signature& _creator){
        this->creator = SignatureHolder(_creator);
        return *this;
    }

    TransactionBuilder& TransactionBuilder::setSigs(const    std::vector<Signature>&& _sigs){
        for(auto&& sig: _sigs){
            sigs.push_back(
                    SignatureHolder(sig)
            );
        }
        return *this;
    }

    TransactionBuilder& TransactionBuilder::addSig(const     Signature&& _sig){
        this->sigs.push_back(
                SignatureHolder(_sig)
        );
        return *this;
    }

    TransactionBuilder& TransactionBuilder::setCreated(const Timestamp& _created){
        this->created = _created;
        return *this;
    }

    TransactionBuilder& TransactionBuilder::setNonce(const   std::vector<uint32_t>& _nonce){
        this->nonce = _nonce;
        return *this;
    }

    TransactionBuilder& TransactionBuilder::setAction(const protocol::Action& _action_type,const  flatbuffers::Offset<void>& _action){
        this->action = _action;
        this->action_type = _action_type;
        return *this;
    }

    std::vector<unsigned char> buildUbyteVector(const char* msg){
        std::vector<unsigned char> res;
        for(int i=0;i < strlen(msg); ++i){
            res.push_back(reinterpret_cast<const unsigned char*>(msg)[i]);
        }
        return res;
    }

    flatbuffers::Offset<Transaction> TransactionBuilder::buildOffset(flatbuffers::FlatBufferBuilder& fbb){

        if(state == State::AfterConstruction){
            throw std::runtime_error("build must be invoked only one time.");
        }
        //creator: Signature (required)
        auto creator_offset = SignatureBuilder(this->creator).buildOffset(fbb);

        std::vector<flatbuffers::Offset<protocol::Signature>> sigs_vec;
        for(auto& sig: this->sigs){
            sigs_vec.push_back(SignatureBuilder(sig).buildOffset(fbb));
        }

        // flatbuffers::Offset<protocol::ActionWrapper>
        std::vector<flatbuffers::Offset<protocol::ActionWrapper>> actions_vec;
        actions_vec.push_back(
                protocol::CreateActionWrapper(
                        fbb,
                        this->action_type,
                        this->action
                )
        );

        state = State::AfterConstruction;
        return protocol::CreateTransactionDirect(
                fbb,
                creator_offset, // flatbuffers::Offset<protocol::Signature> creator = 0,
                &sigs_vec,      // const std::vector<flatbuffers::Offset<protocol::Signature>> *sigs = nullptr,
                this->created,  // uint64_t created = 0,
                0,              // uint32_t nonce = 0,
                &actions_vec,   // std::vector<flatbuffers::Offset<protocol::ActionWrapper>>
                0 // Attachment (but may be removed?)
        );
    }

    flatbuffers::BufferRef<Transaction> TransactionBuilder::buildBufferRef(flatbuffers::FlatBufferBuilder& fbb){
        if(state == State::AfterConstruction){
            throw std::runtime_error("build must be invoked only one time.");
        }

        fbb.Finish(buildOffset(fbb));
        state = State::AfterConstruction;
        return flatbuffers::BufferRef<Transaction>(
                fbb.GetBufferPointer(), fbb.GetSize()
        );
    }

};
