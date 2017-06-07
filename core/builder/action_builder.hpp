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

#ifndef IROHA_ASSET_BUILDER_HPP
#define IROHA_ASSET_BUILDER_HPP

#include <asset_generated.h>
#include <map>
#include <string>

namespace builder{

    using AnyAsset = protocol::AnyAsset;
    using ubyte     = unsigned char;

    class AssetBuilder{
        std::string name;
        std::string uuid;

        AnyAsset  assetType;

        // Currency
        std::vector<ubyte> amount;
        ubyte precision;

        // Complex Asset
        std::vector<ubyte> prop;
        std::vector<ubyte> logic;


    public:
        static std::map<
                AnyAsset,
                std::function<flatbuffers::Offset<void>(
                        flatbuffers::FlatBufferBuilder&,
                        const AssetBuilder&
                )>> buildFunctions;

        AssetBuilder();
        AssetBuilder& as(const AnyAsset&);
        AssetBuilder& setName(const std::string&);
        AssetBuilder& setUuid(const std::string&);

        AssetBuilder& setAmount(const std::vector<ubyte>&);
        AssetBuilder& setPrecision(const ubyte&);

        AssetBuilder& setProp(const std::vector<ubyte>&);
        AssetBuilder& setLogic(const std::vector<ubyte>&);

        flatbuffers::Offset<protocol::Asset> buildOffset(flatbuffers::FlatBufferBuilder& fbb);

    };
};

#endif