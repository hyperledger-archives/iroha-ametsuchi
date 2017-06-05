
#include "flat_iterator.h"


block_store::BlockStoreFlat::Iterator::Iterator(
    block_store::BlockStoreFlat& bstore)
    : bstore_(bstore), n_(0) {}

block_store::BlockStoreFlat::Iterator::Iterator(
    block_store::BlockStoreFlat& bstore, int64_t offset)
    : bstore_(bstore), n_(offset) {}

block_store::BlockStoreFlat::Iterator::Iterator(
    const block_store::BlockStoreFlat::Iterator &it)
    : bstore_(it.bstore_), n_(it.n_), value_(it.value_) {}

const std::vector<uint8_t> &block_store::BlockStoreFlat::Iterator::operator*() {
  value_ = bstore_.get(n_);
  return value_;
}

bool block_store::BlockStoreFlat::Iterator::operator==(
    const block_store::BlockStoreFlat::Iterator &it) {
  return false;
}

bool block_store::BlockStoreFlat::Iterator::operator!=(
    const block_store::BlockStoreFlat::Iterator &it) {
  return n_ != it.n_;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::
operator=(const block_store::BlockStoreFlat::Iterator &it) {
  return *this;
}

bool block_store::BlockStoreFlat::Iterator::operator<(
    const block_store::BlockStoreFlat::Iterator &it) {
  return n_ < it.n_;
}

bool block_store::BlockStoreFlat::Iterator::operator>(const block_store::BlockStoreFlat::Iterator &it) {
  return n_ > it.n_;
}

bool block_store::BlockStoreFlat::Iterator::operator>=(const block_store::BlockStoreFlat::Iterator &it) {
  return n_ >= it.n_;
}

bool block_store::BlockStoreFlat::Iterator::operator<=(const block_store::BlockStoreFlat::Iterator &it) {
  return n_ <= it.n_;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator++() {
  n_ += 1;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator++(int) {
  n_ += 1;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator--() {
  n_ -= 1;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator--(int) {
  n_ -= 1;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator+=(const int &n) {
  n_ += n;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator-=(const int &n) {
  n_ -= n;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator-(const int &n) {
  n_ -= n;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator+(const int &n) {
  n_ += n;
  return *this;
}

Iterator block_store::BlockStoreFlat::begin() {
  return Iterator(*this);
}


Iterator block_store::BlockStoreFlat::end() {
  return Iterator(*this, this->last_id());
}






















