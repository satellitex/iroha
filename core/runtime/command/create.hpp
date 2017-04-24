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
#ifndef IROHA_CREATE_H
#define IROHA_CREATE_H


#include <functional>
#include <generated/commands_generated.h>

namespace runtime{
    namespace create{
        // iroha::AssetCreate
        std::function<Expected<int>(const void*,const std::string&)> assetCreate =
                [](const void* aAssetCreate, const std::string& creator){
                    return 0;
                };
    };
};

#endif //IROHA_CREATE_H
