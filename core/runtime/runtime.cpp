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
#include "runtime.hpp"
#include "validator.hpp"

#include "command/add.hpp"
#include "command/create.hpp"

namespace runtime{

    static std::map<iroha::Command, std::function<Expected<int>(const void*,const std::string&)>>
        command_process;

    void initialize(){
        // Add
        {
            command_process[iroha::Command::Command_AccountAdd] = add::accountAdd;
            command_process[iroha::Command::Command_AssetAdd] = add::assetAdd;
            command_process[iroha::Command::Command_ChaincodeAdd] = add::chaincodeAdd;
            command_process[iroha::Command::Command_PeerAdd] = add::peerAdd;
        }
        // Create
        {
            command_process[iroha::Command::Command_AssetCreate] = create::assetCreate;
        }
    }

    void processTransaction(const iroha::Transaction& tx){
        if(!validator::account_exist_validator(tx)){
            // Reject
        }
        if(!validator::permission_validator(tx)){
            // Reject
        }
        if(!validator::logic_validator(tx)){
            // Reject
        }

        if(command_process.count(tx.command_type()) != 0){
             command_process[tx.command_type()](tx.command(),tx.creatorPubKey()->c_str());
        }
    }

};