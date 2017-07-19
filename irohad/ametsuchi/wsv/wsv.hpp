/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
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

#ifndef AMETSUCHI_WSV_WSV_HPP
#define AMETSUCHI_WSV_WSV_HPP

#include <ametsuchi/wrapper/rdb_wrapper.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace ametsuchi {

  namespace wsv {

    class WSV {
     public:

      bool append(Transaction tx);
      bool commit();
      bool rollback();

      // temporaryWSV  return snapshot;
      WSV temporaryWSV();

      // Get Series
      Account getAccount(std::string account_uuid);
      Asset getAsset(std::string asset_uuid);
      std::vector<Asset> getAssets(std::string domain);
      Wallet getWallet(std::string account_uuid, std::string asset_uuid);
      AssetPermission getAssetPermission(std::string account_uuid, std::string asset_uuid);
      DomainPermission getDomainPermission(std::string account_uuid, std::string domain);
      std::vector<Peer> getPeers();

     private:
      RDBWrapper db_;
    };

  }  // namespace wsv

}  // namespace ametsuchi
#endif  // AMETSUCHI_WSV_WSV_HPP
