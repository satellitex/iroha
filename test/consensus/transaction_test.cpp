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
#include <vector>
#include <memory>
#include <thread>

#include <consensus/sumeragi.hpp>

#include <infra/config/peer_service_with_json.hpp>
#include "../../tools/helper/issue_transaction_add.hpp"


void setAwkTimer(int const sleepMillisecs, std::function<void(void)> const &action) {
    std::thread([action, sleepMillisecs]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMillisecs));
        action();
    }).join();
}

int main(){
    std::string senderPublicKey;

    std::string pubKey = ::peer::myself::getPublicKey();

    int i = 0;
    while(1){
        setAwkTimer(30, [&i]() {
            std::string command = "add";
            std::string data = "domain";
            std::vector<std::string> params {"domain " + std::to_string(i), "key " + std::to_string(i)};
            tools::issue_transaction::add::domain::issue_transaction(params);
            ++i;
        });
    }

    return 0;
}
