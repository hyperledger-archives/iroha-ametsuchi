#include <cpp_redis/cpp_redis>

int main() {
  cpp_redis::redis_client client;

client.connect();

  client.set("hello", "42");
  client.get("hello",
             [](cpp_redis::reply& reply) { std::cout << reply << std::endl; });
  client.sync_commit();
  return 0;
}