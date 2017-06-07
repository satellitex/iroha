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


#include <grpc++/grpc++.h>
#include <consensus/block_builder.hpp> // sumeragi::Block
#include <infra/config/iroha_config_with_json.hpp> // createChannel()
#include <membership_service/peer_service.hpp> // ::peer::service::getActivePeerList()
#include <validator/stateless_validator.hpp> // validator::stateless::receive()
#include <service/flatbuffer_service.hpp>
#include <endpoint_generated.h>
#include <endpoint.grpc.fb.h> // protocol::Sumeragi

namespace connection {

enum class ResponseType {
  RESPONSE_OK,
  RESPONSE_INVALID_SIG,  // wrong signature
  RESPONSE_ERRCONN,      // connection error
};

/**
 * helper function to create channel
 */
static std::shared_ptr<grpc::Channel> createChannel(const std::string& serverIp/*, GrpcPortType portType*/) {
  return grpc::CreateChannel(
    serverIp + ":" +
    std::to_string(config::IrohaConfigManager::getInstance()
                     .getGrpcPortNumber(50051)),
    grpc::InsecureChannelCredentials()
  );
}

/**
 * SumeragiServer
 * @brief RPC server.
 * @details UnPacks flatbuf and dispatches it to stateless-validation.
 *          sumeragi -> interface -> RPC Client -> [RPC Server] -> stateless-validation -> sumeragi
*/
class SumeragiServer : public protocol::Sumeragi::Service {
  virtual grpc::Status Communicate(
    grpc::ClientContext* context,
    const flatbuffers::BufferRef<protocol::Block>& request,
    flatbuffers::BufferRef<protocol::SumeragiResponse>* response) {

    // UnPack to sumeragi::Block
    sumeragi::Block block;
    block.unpackBlock(request.buf, request.len);

    fbbResponse.Clear();

    // Test stateless validator
    // If test succeeded, dispatch block to sumeragi.
    auto valid = validator::stateless::test(block);

    if (!valid) {
      const std::string message = "Stateless validation failed";
      *response = flatbuffer_service::endpoint::CreateSumeragiResponseRef(
        fbbResponse, message, protocol::ResponseCode::FAIL
      );
      return grpc::Status::CANCELLED;
    }

    // Returns signature
    const std::string message = "Success receive tx";
    *response = flatbuffer_service::endpoint::CreateSumeragiResponseRef(
      fbbResponse, message, protocol::ResponseCode::FAIL
    );
    return grpc::Status::OK;
  }

private:
  flatbuffers::FlatBufferBuilder fbbResponse;
};

/**
 * SumeragiClient
 * @brief RPC client connects with other RPC server.
 */
class SumeragiClient {
public:
  explicit SumeragiClient(const std::string& serverIp)
    : stub_(protocol::Sumeragi::NewStub(createChannel(serverIp))) {}

  /**
   * Communicate
   * @detail sumeragi -> interface -> [RPC Client] -> RPC Server -> stateless-validation -> sumeragi
   * @param block_ - block to consensus
   * @param response - to return SumeragiServer response
   *
   * @return grpc::Status
   */
  grpc::Status Communicate(const sumeragi::Block &block,
                           flatbuffers::BufferRef<protocol::SumeragiResponse> *response) const {
    grpc::ClientContext context;
    flatbuffers::FlatBufferBuilder fbb;
    auto request = block.packBufferRef(fbb);
    return stub_->Communicate(&context, request, response);
  }

private:
  std::unique_ptr<protocol::Sumeragi::Stub> stub_;
};

namespace with_sumeragi {

/**
 * unicast()
 * @brief unicasts block to a validating peer.
 * @param block - block to consensus.
 * @param index - validating peer's index.
 */
void unicast(const sumeragi::Block& block, ::peer::Nodes::const_iterator iter) {

  if (::peer::service::isExistIP((*iter)->ip)) {
    SumeragiClient client((*iter)->ip);

    flatbuffers::BufferRef<protocol::SumeragiResponse> response;
    auto stat = client.Communicate(block, &response);

    // current implementation doesn't require response.

    if (!stat.ok()) {
      std::cout << "{error_code: " << stat.error_code() << ", "
                << "error_message: " << stat.error_message() << "}\n";
    }
  } else {
    std::cout << "IP: " << (*iter)->ip << " doesn't exist." << std::endl;
  }
}

/**
 * multicast()
 * @brief multicasts block to [beginIndex, endIndex) peers.
 * @param block - block to consensus.
 * @param begin - validating peer's iterator except leader (usually next to begin())
 * @param end - validating peer's tail + 1 iterator
 */
void multicast(const sumeragi::Block& block,
               ::peer::Nodes::const_iterator begin,
               ::peer::Nodes::const_iterator end) {
  for (auto iter = begin; iter != end; iter++) {
    unicast(block, iter);
  }
}

/**
 * commit()
 * @brief commits block to all peers including sender.
 * @param block - block to consensus.
 */
void commit(const sumeragi::Block& block) {
  auto peers = ::peer::service::getActivePeerList();
  for (auto iter = peers.begin(); iter != peers.end(); iter++) {
    unicast(block, iter);
  }
}

} // namespace with_sumeragi

class ClientService : public protocol::ClientService::Service {
public:
  ClientService(const std::string& serverIp)
    : stub_(protocol::ClientService::NewStub(createChannel(serverIp))) {}

  /**
   * Torii
   * @brief Iroha receives a tx from gRPC clients.
   * @param context - has client's ip and port
   * @param request - contains tx to consensus
   * @param response - has signature of this peer that guarantees completion of receiving tx.
   * @return grpc::Status
   */
  virtual grpc::Status Torii(
    grpc::ServerContext* context,
    const flatbuffers::BufferRef<protocol::Transaction>* request,
    flatbuffers::BufferRef<protocol::SumeragiResponse>* response)
  {

  }

  /**
   * ToriiBatch
   * @brief Iroha receives batched txs from gRPC clients.
   * @param context
   * @param request
   * @param response
   * @return grpc::Status
   */
  virtual ::grpc::Status ToriiBatch(
    grpc::ClientContext* context,
    const flatbuffers::BufferRef<protocol::TransactionBatch>& request,
    flatbuffers::BufferRef<protocol::SumeragiResponse>* response)
  {
    throw std::runtime_error("Not implemented");
  }

  // virtual grpc::Status Query(...) {}

private:
  std::unique_ptr<protocol::ClientService::Stub> stub_;
};

class PeerServiceClient {
public:
  PeerServiceClient(const std::string& serverIp)
    : stub_(protocol::ClientService::NewStub(createChannel(serverIp))) {}

  /**
   * Torii
   * @brief Peer services use this to send a tx to other peer's sumeragi.
   * @details peer service -> interface -> [RPC Cient] -> RPC Server -> other sumeragi
   *
   * @param tx [description]
   * @param responseRef [description]
   *
   * @return grpc::Status
   */
  grpc::Status Torii(const flatbuffers::BufferRef<protocol::Transaction> &request,
                     flatbuffers::BufferRef<protocol::SumeragiResponse> *response) const {
    grpc::ClientContext context;
    return stub_->Torii(&context, request, response);
  }

private:
  std::unique_ptr<protocol::ClientService::Stub> stub_;
};


} // namespace connection
