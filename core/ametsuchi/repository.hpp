/**
 * Copyright Soramitsu Co., Ltd. 2016 All Rights Reserved.
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

#ifndef IROHA_REPOSITORY_H
#define IROHA_REPOSITORY_H

#include <main_generated.h>

namespace repository {


    void init();
    void append(const iroha::Transaction &tx);
    const ::iroha::Transaction *getTransaction(size_t index);
    std::vector<const protocol::Asset *> findAssetByPublicKey();
    bool existAccountOf(const flatbuffers::String &key);
    bool checkUserCanPermission(const flatbuffers::String &key);
    const std::string getMerkleRoot();

};

#endif  // IROHA_REPOSITORY_H
