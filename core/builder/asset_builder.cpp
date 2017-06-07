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



#include "asset_builder.hpp"
#include "build_validator.hpp"

namespace builder{

    AssetBuilder::AssetBuilder():
            precision(0)
    {}

    AssetBuilder& AssetBuilder::as(const AnyAsset& _type){
        this->assetType = _type;
        return *this;
    }

    AssetBuilder& AssetBuilder::setName(const std::string& _name){
        this->name = _name;
        return *this;
    }
    AssetBuilder& AssetBuilder::setUuid(const std::string& _uuid){
        this->uuid = _uuid;
        return *this;
    }

    AssetBuilder& AssetBuilder::setAmount(const std::vector<ubyte>& _amount){
        this->amount = _amount;
        return *this;
    }
    AssetBuilder& AssetBuilder::setPrecision(const ubyte& _precision){
        this->precision = _precision;
        return *this;
    }

    AssetBuilder& AssetBuilder::setProp(const std::vector<ubyte>& _prop){
        this->prop = _prop;
        return *this;
    }
    AssetBuilder& AssetBuilder::setLogic(const std::vector<ubyte>& _logic){
        this->logic = _logic;
        return *this;
    }

    flatbuffers::Offset<protocol::Asset> AssetBuilder::buildOffset(flatbuffers::FlatBufferBuilder& fbb){
        return CreateAssetDirect(fbb,
                                 this->name.c_str(),
                                 this->uuid.c_str(),
                                 this->assetType,
                                 buildFunctions[this->assetType]( fbb, *this)
        );
    }

    std::map<
            AnyAsset,
            std::function<flatbuffers::Offset<void>(
            flatbuffers::FlatBufferBuilder&,
    const AssetBuilder&
    )>>
    AssetBuilder::buildFunctions = {
            { AnyAsset::Currency, [&](flatbuffers::FlatBufferBuilder& fbb, const AssetBuilder& builder) -> flatbuffers::Offset<void>{
                // === Require validator ===
                isSufficeUbyteVector(builder.amount,"amount");
                // =========================
                return protocol::CreateCurrencyDirect(fbb,
                                                      &builder.amount,
                                                      builder.precision
                ).Union();
            }},
            { AnyAsset::ComplexAsset, [&](flatbuffers::FlatBufferBuilder& fbb, const AssetBuilder& builder) -> flatbuffers::Offset<void>{
                // === Require validator ===
                isSufficeUbyteVector(builder.prop,"prop");
                // =========================
                return protocol::CreateComplexAssetDirect(fbb,
                                                          &builder.prop
                ).Union();
            }},
    };
};