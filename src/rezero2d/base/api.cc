// Created by DONG Zhong on 2024/02/13.

#include "rezero2d/base/api.h"

namespace rezero {

namespace endian {

union {
  std::uint32_t n;
  std::uint8_t p[4];
} kEndianTest = {0x01020304};

} // endian

Endianness GetEndianOrder() {
  return (0x04 == endian::kEndianTest.p[0])
             ? Endianness::kLittleEndian
             : (0x01 == endian::kEndianTest.p[0])
                   ? Endianness::kBigEndian : Endianness::kUnknown;
}

} // namespace rezero
