//
// Created by hdalpha on 05.06.17.
//

#ifndef AMETSUCHI_ITERATOR_H
#define AMETSUCHI_ITERATOR_H

#include "block_store_flat.h"

namespace block_store{

class BlockStoreFlat::Iterator{

 public:
  Iterator(BlockStoreFlat& bstore);
  Iterator(BlockStoreFlat& bstore, int64_t offset);
  Iterator(const Iterator& it);
  // Operators
  bool operator==(const BlockStoreFlat::Iterator &it);
  bool operator!=(const BlockStoreFlat::Iterator &it);
  Iterator operator=(const BlockStoreFlat::Iterator &it);
  bool operator<(const BlockStoreFlat::Iterator &it);
  bool operator>(const BlockStoreFlat::Iterator &it);
  bool operator>=(const BlockStoreFlat::Iterator &it);
  bool operator<=(const BlockStoreFlat::Iterator &it);

  const std::vector<uint8_t> &operator*();

  Iterator operator++();
  Iterator operator++(int);
  Iterator operator--();
  Iterator operator--(int);

  Iterator operator+=(const int &n);
  Iterator operator-=(const int &n);
  Iterator operator-(const int &n);
  Iterator operator+(const int &n);

 protected:
  BlockStoreFlat& bstore_;
  int64_t n_;
  std::vector<uint8_t> value_;

};



}

#endif //AMETSUCHI_ITERATOR_H
