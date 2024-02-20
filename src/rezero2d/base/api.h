// Created by DONG Zhong on 2024/02/13.

#ifndef REZERO_BASE_API_H_
#define REZERO_BASE_API_H_

#include <cstdint>

namespace rezero {

enum class Endianness : std::uint32_t {
  kLittleEndian = 0x00000001,
  kBigEndian    = 0x01000000,
  kUnknown      = 0xFFFFFFFF,
};

Endianness GetEndianOrder();

} // namespace rezero

#endif // REZERO_BASE_API_H_
