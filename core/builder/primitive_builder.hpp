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

#ifndef IROHA_PRIMITIVE_BUILDER_HPP
#define IROHA_PRIMITIVE_BUILDER_HPP

#include <primitives_generated.h>
#include <vector>

namespace builder{

    using Signature   = protocol::Signature;

    struct SignatureHolder{
        std::vector<unsigned char> pubkey;
        std::vector<unsigned char> sig;
        SignatureHolder(){};
        SignatureHolder(
                const char* _pubkey,
                const char* _sig
        ){
            for(int i = 0;i < strlen(_pubkey); ++i){
                pubkey.push_back(reinterpret_cast<const unsigned char*>(_pubkey)[i]);
            }
            for(int i = 0;i < strlen(_sig); ++i){
                sig.push_back(reinterpret_cast<const unsigned char*>(_sig)[i]);
            }
        }
        SignatureHolder(const Signature& _sig){
            for(auto c: *_sig.pubkey()){
                pubkey.push_back(c);
            }
            for(auto c: *_sig.sig()){
                sig.push_back(c);
            }
        }
    };

    class SignatureBuilder{
        std::vector<unsigned char> pubkey;
        std::vector<unsigned char> sig;
    public:
        SignatureBuilder(){}
        SignatureBuilder(const SignatureHolder& holder):
                pubkey(holder.pubkey),
                sig(holder.sig)
        {}
        flatbuffers::Offset<Signature> buildOffset(flatbuffers::FlatBufferBuilder& fbb){
            return protocol::CreateSignatureDirect(fbb,
                                                   &this->pubkey,
                                                   &this->sig
            );
        }
        flatbuffers::BufferRef<Signature> buildBufferRef(flatbuffers::FlatBufferBuilder& fbb){
            fbb.Finish(buildOffset(fbb));
            return flatbuffers::BufferRef<Signature>(
                    fbb.GetBufferPointer(), fbb.GetSize()
            );
        }
    };

};

#endif