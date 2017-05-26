#include <ametsuchi/block_index/block_index_redis.h>
namespace ametsuchi{
namespace block_index{

BlockIndexRedis::~BlockIndexRedis() {
  client_.disconnect();
}


}
}
