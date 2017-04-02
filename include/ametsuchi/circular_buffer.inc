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

template <typename T>
CircularBuffer<T>::CircularBuffer(size_t s) : cap(s), sz(0), i_end(0) {
  if (s == 0) throw Exception("Buffer size cannot be zero");
  v = (T *)malloc(sizeof(T) * capacity());
}

template <typename T>
CircularBuffer<T>::CircularBuffer(CircularBuffer &&cs) {
  v = cs.v;
  cap = cs.cap;
  sz = cs.sz;
  i_end = cs.i_end;
  cs.v = nullptr;
}

template <typename T>
CircularBuffer<T>::~CircularBuffer() {
  if (v) free(v);
  v = nullptr;
}

template <typename T>
void CircularBuffer<T>::resize(size_t s) {
  cap = s;
  sz = std::min(capacity(), size());
  v = (T *)realloc(v, sizeof(T) * capacity());
}

template <typename T>
T CircularBuffer<T>::push(T &&t) {
  T tmp = v[start()];
  v[i_end] = std::move(t);
  i_end = (i_end + 1) % capacity();
  if (size() < capacity()) sz++;
  return tmp;
}

template <typename T>
T CircularBuffer<T>::push(const T &t) {
  T tmp = v[start()];
  v[i_end] = t;
  i_end = (i_end + 1) % capacity();
  if (size() < capacity()) sz++;
  return tmp;
}

template <typename T>
void CircularBuffer<T>::pop(size_t n) {
  sz = size() < n ? 0 : size() - n;
}

template <typename T>
T &CircularBuffer<T>::operator[](size_t s) {
  if (s >= size()) throw Exception("Buffer accessing out of size");
  return v[diff(i_end, size() - s)];
}

template <typename T>
T CircularBuffer<T>::operator[](size_t s) const {
  if (s >= size()) throw Exception("Buffer accessing out of size");
  return v[diff(i_end, size() - s)];
}

template <typename T>
CircularBuffer<T>::ForwardIter::ForwardIter(CircularBuffer<T> *cs, size_t pos)
    : cs(cs), pos(pos) {}

template <typename T>
bool CircularBuffer<T>::ForwardIter::operator==(const ForwardIter &i) const {
  return cs->v == i.cs->v && pos == i.pos;
}

template <typename T>
bool CircularBuffer<T>::ForwardIter::operator!=(const ForwardIter &i) const {
  return !this->operator==(i);
}

template <typename T>
typename CircularBuffer<T>::ForwardIter
    &CircularBuffer<T>::ForwardIter::operator++() {
  pos = pos_inc(1);
  return *this;
}

template <typename T>
typename CircularBuffer<T>::ForwardIter CircularBuffer<T>::ForwardIter::operator+(
    size_t t) {
  return {cs, pos_inc(t)};
}

template <typename T>
typename CircularBuffer<T>::ForwardIter &CircularBuffer<T>::ForwardIter::
operator+=(size_t t) {
  pos = pos_inc(t);
  return *this;
}

template <typename T>
T &CircularBuffer<T>::ForwardIter::operator*() {
  return (*cs)[pos];
}

template <typename T>
T &CircularBuffer<T>::ForwardIter::operator[](size_t t) {
  return (*cs)[pos_inc(t)];
}

template <typename T>
size_t CircularBuffer<T>::ForwardIter::pos_inc(size_t t) {
  // temporal end, that bigger current position
  auto t_end = cs->i_end + pos >= cs->i_end ? cs->size() : 0;
  // calculate new position
  auto n_pos = pos + t;
  // ensure it doesn't overflow
  return n_pos >= t_end ? cs->overflowed() : n_pos % cs->cap;
}
