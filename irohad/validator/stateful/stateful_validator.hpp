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
#ifndef IROHA_VALIDATION_STATEFUL_VALIDATOR_HPP
#define IROHA_VALIDATION_STATEFUL_VALIDATOR_HPP

#include <ametsuchi/temporary_wsv.hpp>
#include <model/proposal.hpp>

namespace validator {
  namespace stateful {

    /**
     * Interface for performing stateful validator
     */
    class StatefulValidator {
     public:
      StatefulValidator(const WSV& wsv);
      bool validate(const Transaction& tx);
    };
  }  // namespace validator
}  // namespace iroha
#endif  // IROHA_VALIDATION_STATELESS_VALIDATOR_HPP
