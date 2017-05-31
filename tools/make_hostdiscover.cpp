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

#include <endpoint_generated.h>
#include <grpc++/create_channel.h>
#include <service/flatbuffer_service.h>
#include <infra/config/iroha_config_with_json.hpp>
#include <infra/config/peer_service_with_json.hpp>
#include <service/connection.hpp>
#include <utils/ip_tools.hpp>

int main(int argc, char* argv[]) {
  std::string discoveredFileName = "discovered.txt";
  if (argc == 2) discoveredFileName.assign(argv[1]);
  if (argc > 2) {
    std::cout << "Usage: make_hostdiscover [filename]" << std::endl;
    std::cout << "Discovered nodes will be saved into filename "
                 "in <ip> <public key> format"
              << std::endl;
    return 1;
  }

  std::vector<std::string> defaultHosts{};
  auto trustedHosts =
      config::IrohaConfigManager::getInstance().getTrustedHosts(defaultHosts);
  if (trustedHosts.empty()) {
    std::cout << "Config section for trusted hosts was not found or empty."
              << std::endl;
    return 1;
  }

  std::ofstream hostsDiscovered(discoveredFileName);

  auto vec = flatbuffer_service::endpoint::CreatePing(
      "discovery", config::PeerServiceConfig::getInstance().getMyIp());
  auto& ping = *flatbuffers::GetRoot<iroha::Ping>(vec.data());


  for (auto host : trustedHosts) {
    std::string pubkey;
    if (ip_tools::isIpValid(host)) {
      if (connection::memberShipService::HijiriImpl::Kagami::send(host, ping,
                                                                  pubkey)) {
        hostsDiscovered << host << " " << pubkey << std::endl;
      }
    } else {
      // maybe we have a netmask?
      auto range = ip_tools::getIpRangeByNetmask(host);
      for (uint32_t i = 0; i < range.second; ++i) {
        if (connection::memberShipService::HijiriImpl::Kagami::send(
                ip_tools::uintIpToString(range.first++), ping, pubkey)) {
          hostsDiscovered << ip_tools::uintIpToString(range.first) << " "
                          << pubkey << std::endl;
        }
      }
    }
  }

  return 0;
}
