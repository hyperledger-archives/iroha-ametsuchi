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

#include <ametsuchi/ametsuchi.h>
#include <grpc++/grpc++.h>
#include <gtest/gtest.h>

class ClientTest : public ::testing::Test {
 protected:
  class FakeStorageServiceImpl final : public iroha::Storage::Service {
   public:

    void test_add(const ::iroha::BlockMessage *request){
      iroha::Block block;
      block.mutable_meta()->set_height(1);
      ASSERT_EQ(request->block().Utf8DebugString(), block.Utf8DebugString());
    }
    grpc::Status AddBlock(::grpc::ServerContext *context,
                          const ::iroha::BlockMessage *request,
                          ::google::protobuf::Empty *response) override {
      test_add(request);
      return grpc::Status::OK;
    }


    grpc::Status GetPeers(::grpc::ServerContext *context,
                          const ::google::protobuf::Empty *request,
                          ::iroha::PeersResponse *response) override {
      response->add_address("127.0.0.1");
      return grpc::Status::OK;
    }


    void test_get(const iroha::BlockIdMessage *request) {
      ASSERT_TRUE(request->id_case() == iroha::BlockIdMessage::kHeight ||
                  request->id_case() == iroha::BlockIdMessage::kBlockHash ||
                  request->id_case() ==
                      iroha::BlockIdMessage::kTransactionHash);
    }
    grpc::Status GetBlock(::grpc::ServerContext *context,
                          const ::iroha::BlockIdMessage *request,
                          ::iroha::BlockMessage *response) override {
      auto block = response->mutable_block();
      block->mutable_meta()->set_height(2);
      return grpc::Status::OK;
    }


    void test_erase(const iroha::BlockIdMessage *request) {
      ASSERT_EQ(request->id_case(), iroha::BlockIdMessage::kHeight);
      ASSERT_EQ(request->height(), 0);
    }
    grpc::Status EraseBlock(::grpc::ServerContext *context,
                            const ::iroha::BlockIdMessage *request,
                            ::google::protobuf::Empty *response) override {
      test_erase(request);
      return grpc::Status::OK;
    }

    grpc::Status GetBlocks(
        ::grpc::ServerContext *context, const ::iroha::RangeGetRequest *request,
        ::grpc::ServerWriter<::iroha::BlockMessage> *writer) override {
      iroha::Block block;
      iroha::BlockMessage message;
      message.set_allocated_block(&block);
      writer->Write(message);
      return grpc::Status::OK;
    }
  };
};

TEST_F(ClientTest, sample_test) {
  std::string server_address("0.0.0.0:50051");
  FakeStorageServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  auto server = builder.BuildAndStart();

  ametsuchi::Client client;

  iroha::Block block;
  block.mutable_meta()->set_height(1);

  ASSERT_TRUE(client.add(&block));
  ASSERT_EQ(block.meta().height(), 1); // block not deleted

  auto peers = client.get_peers();
  ASSERT_EQ(peers.size(), 1);
  ASSERT_EQ(peers.at(0), "127.0.0.1");

  ASSERT_TRUE(client.erase(0));
  server->Shutdown();
}
