/**
 * Copyright Soramitsu Co., Ltd. 2016 All Rights Reserved.
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

#ifndef AMETSUCHI_SERIALIZER_H
#define AMETSUCHI_SERIALIZER_H

#include <cstring>

namespace ametsuchi {
namespace serialize {

/**
 * WARNING!!!
 * Be very careful with these macroses. They work really fast, but don't have
 * any validation checks.
 */

/**
 * Use this macros to serialize any primitive (e.g. uint*_t) types.
 * @param dst destination pointer
 * @param value source element
 * @param type type of src
 *
 * For code example look at serialize_test
 */
#define PUT_UINT(dst, value, type)          \
  {                                         \
    *reinterpret_cast<type *>(dst) = value; \
    dst += sizeof(type);                    \
  }

/**
 * Use this macros to deserialize any primitive (e.g. uint*_t) types.
 * The opposite to PUT_UINT
 * @param dst destination pointer
 * @param src source pointer
 * @param type destination type
 *
 * For code example look at serialize_test
 */
#define GET_UINT(dst, src, type)         \
  {                                      \
    std::memcpy(dst, src, sizeof(type)); \
    src += sizeof(type);                 \
  }

/**
 * Use this macros to serialize ByteArray of given size (bytes)
 * @param dst destination pointer, will be incremented after serialization
 * @param array source -- ByteArray
 *
 * For code example look at serialize_test
 */
#define PUT_BYTE_ARRAY(dst, array)                                             \
  {                                                                            \
    size_t size = array.size();                                                \
    PUT_UINT(dst, size, size_t);                                               \
    std::memcpy(dst,                                                           \
                reinterpret_cast<const ByteArray::value_type *>(array.data()), \
                size);                                                         \
    dst += size;                                                               \
  }

/**
 * Use this macros to deserialize any array of given size (bytes)
 * @param dst destination pointer -- ByteArray&
 * @param src source pointer -- uint8_t*
 *
 * For code example look at serialize_test
 */
#define GET_BYTE_ARRAY(dst, src)   \
  {                                \
    size_t n = 0;                  \
    GET_UINT(&n, src, size_t);     \
    dst = ByteArray{src, src + n}; \
    src += n;                      \
  }


template <typename T>
class Serializer {
  virtual ByteArray serialize(const T *obj) = 0;
  virtual T deserialize(const ByteArray *blob) = 0;
};
}
}

#endif  // AMETSUCHI_SERIALIZER_H
