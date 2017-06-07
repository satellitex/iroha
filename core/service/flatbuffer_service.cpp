/**
* Copyright 2017 Soramitsu Co., Ltd.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <flatbuffers/flatbuffers.h>
#include <primitives_generated.h> // Signature
#include <endpoint_generated.h> // SumeragiResponse
#include <crypto/signature.hpp> // sign
#include <crypto/base64.hpp> // decode
#include <infra/config/peer_service_with_json.hpp> // PeerServiceConfig

namespace flatbuffer_service {

namespace primitives {

flatbuffers::Offset<protocol::Signature> CreateSignature(
  flatbuffers::FlatBufferBuilder &fbb, const std::string &hash
) {
  const auto pk = base64::decode(
    config::PeerServiceConfig::getInstance().getMyPublicKey()
  );
  const auto sk = base64::decode(
    config::PeerServiceConfig::getInstance().getMyPrivateKey()
  );

  const auto signature = signature::sign(
    hash, pk, sk
  );

  return protocol::CreateSignatureDirect(fbb, &pk, &signature);
}

} // namespace primitives

namespace endpoint {

flatbuffers::BufferRef<protocol::SumeragiResponse> CreateSumeragiResponseRef(
  flatbuffers::FlatBufferBuilder &fbb,
  const std::string& message,
  protocol::ResponseCode code
) {

  // Use message instead of tx hash.
  auto signature = primitives::CreateSignature(fbb, message);

  auto response = protocol::CreateSumeragiResponse(
    fbb, fbb.CreateString(message),
    code, signature
  );
  fbb.Finish(response);
  return flatbuffers::BufferRef<protocol::SumeragiResponse>(
    fbb.GetBufferPointer(), fbb.GetSize()
  );
}

} // namespace endpoint

namespace verifier {
  bool VerifyBlock(const uint8_t* flatbuf, size_t length) {
    flatbuffers::Verifier verifier(flatbuf, length);
    return verifier.VerifyBuffer<protocol::Block>(nullptr);
  }
} // namespace verifier

} // namespace flatbuffer_service
