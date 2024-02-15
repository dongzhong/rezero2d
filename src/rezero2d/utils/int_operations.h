// Created by DONG Zhong on 2024/02/14.

#ifndef REZERO_UTILS_INT_OPERATIONS_H_
#define REZERO_UTILS_INT_OPERATIONS_H_

#include <cstdint>

namespace rezero {

template <typename T>
static inline T ByteSwap16(const T& t) {
  // TOOD:
  return T((std::uint16_t(t) << 8) | (std::uint16_t(t) >> 8));
}

template <typename T>
static inline T ByteSwap32(const T& t) {
  // TODO:
  return T((std::uint32_t(t) << 24) | (std::uint32_t(t) >> 24) |
           ((std::uint32_t(t) << 8) & 0x00FF0000) | ((std::uint32_t(t) >> 8) & 0x0000FF00));
}

template <typename T>
static inline T ByteSwap64(const T& t) {
  // TODO:
  return T(std::uint64_t(ByteSwap32(std::uint32_t(std::uint64_t(t) >> 32))) |
           std::uint64_t(ByteSwap32(std::uint32_t((std::uint64_t(t) & 0xFFFFFFFF) << 32))));
}

template <typename T>
static inline T ByteSwap(const T& t) {
  static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 | sizeof(T) == 8);

  if constexpr (sizeof(T) == 1) {
    return t;
  } else if constexpr (sizeof(T) == 2) {
    return ByteSwap16(t);
  } else if constexpr (sizeof(T) == 4) {
    return ByteSwap32(t);
  } else {
    return ByteSwap64(t);
  }
}

} // namespace rezero

#endif // REZERO_UTILS_INT_OPERATIONS_H_
