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

#include <string>
#include <asset_generated.h>
#include <endpoint_generated.h>
#include <infra/config/config_utils.hpp>
#include <main_generated.h>
#include <crypto/hash.hpp>
#include <service/flatbuffer_service.h>
#include <service/connection.hpp>
#include <string>
#include <memory>
#include <sys/stat.h>


namespace repository {


void init() {
    // WIP
}

void append(const iroha::Transaction &tx) {
    // WIP
}

const ::iroha::Transaction *getTransaction(size_t index) {
    // WIP
}

std::vector<const protocol::Asset *> findAssetByPublicKey(){
    // WIP
}

bool existAccountOf(const flatbuffers::String &key) {
  return false;
}

bool checkUserCanPermission(const flatbuffers::String &key) {
  return false;
}

const std::string getMerkleRoot() {
}

};