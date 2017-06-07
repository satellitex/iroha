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

#include "build_validator.hpp"

namespace builder{

    bool isSufficeStr(const std::string& str,const std::string& name){
        if(str == ""){
            throw std::runtime_error("Error! "+name+" should be required");
        }
        return true;
    }

    bool isSufficeStr(const std::string& str,const std::string& name, const std::string& pattern){
        try {
            std::regex re(pattern);
            if(!std::regex_match(str, re)){
                throw std::runtime_error("Error! username should Suffice " + pattern);
            }
            return true;
        } catch (const std::regex_error& e) {
            std::cout <<e.code() <<":"<< e.what() << '\n';
        }
        return false;
    }

    bool isSufficeSignatureHolder(const std::vector<SignatureHolder>& sigs,const std::string& name){
        if(sigs.empty()){
            throw std::runtime_error("Error! "+name+" should be required");
        }
        return true;
    }

    bool isSufficeUbyteVector(const std::vector<ubyte>& blob,const std::string& name){
        if(blob.empty()){
            throw std::runtime_error("Error! "+name+" should be required");
        }
        return true;
    }

}
