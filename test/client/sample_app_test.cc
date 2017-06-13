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
    void test_add(const ::iroha::BlockMessage *request) {
      ASSERT_EQ(1, request->block().meta().height());
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
      switch (request->id_case()) {
        case iroha::BlockIdMessage::kHeight: {
          ASSERT_EQ(request->height(), 1);
          break;
        }
        case iroha::BlockIdMessage::kBlockHash: {
          ASSERT_EQ(request->block_hash(), "0");
          break;
        }
        case iroha::BlockIdMessage::kTransactionHash: {
          ASSERT_EQ(request->transaction_hash(), "1");
          break;
        }
        case iroha::BlockIdMessage::ID_NOT_SET: {
          FAIL();
        }
        default: { FAIL(); }
      }
    }
    grpc::Status GetBlock(::grpc::ServerContext *context,
                          const ::iroha::BlockIdMessage *request,
                          ::iroha::BlockMessage *response) override {
      test_get(request);
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

    void test_get(const iroha::RangeGetRequest *request){
      ASSERT_EQ(request->begin(), 1);
      ASSERT_EQ(request->end(), 1);
    }
    void test_get_write(grpc::ServerWriter<iroha::BlockMessage> *writer){
      iroha::BlockMessage message;
      auto block = message.mutable_block();
      block->mutable_meta()->set_height(2);
      ASSERT_TRUE(writer->Write(message));
    }
    grpc::Status GetBlocks(
        ::grpc::ServerContext *context, const ::iroha::RangeGetRequest *request,
        ::grpc::ServerWriter<::iroha::BlockMessage> *writer) override {
      test_get(request);
      test_get_write(writer);
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
  ASSERT_TRUE(client.server_alive(std::chrono::system_clock::now() +
                                  std::chrono::seconds(1)));

  iroha::Block block;
  block.mutable_meta()->set_height(1);

  // Add block
  ASSERT_TRUE(client.add(&block));
  ASSERT_EQ(block.meta().height(), 1);  // block not deleted

  // Get peers
  auto peers = client.get_peers();
  ASSERT_EQ(peers.size(), 1);
  ASSERT_EQ(peers.at(0), "127.0.0.1");

  // Erase block
  ASSERT_TRUE(client.erase(0));

  // Get by height, block hash, transaction hash
  iroha::Block received_block;
  received_block = client.get_by_height(1);
  ASSERT_EQ(received_block.meta().height(), 2);
  received_block = client.get_by_block_hash("0");
  ASSERT_EQ(received_block.meta().height(), 2);
  received_block = client.get_by_transaction_hash("1");
  ASSERT_EQ(received_block.meta().height(), 2);

  // Get block range
  auto stream = client.get_range(1, 1);
  ASSERT_TRUE(stream >> received_block);
  ASSERT_EQ(received_block.meta().height(), 2);
  ASSERT_FALSE(stream >> received_block);

  server->Shutdown();
}
