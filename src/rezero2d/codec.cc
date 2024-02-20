// Created by DONG Zhong on 2024/02/13.

#include "rezero2d/codec.h"

#include "rezero2d/codec/bmp_codec.h"

namespace rezero {

std::shared_ptr<Codec> Codec::GetCodec(CodecType type) {
  switch (type) {
    case CodecType::kBMP:
    default:
      return std::make_shared<BMPCodec>();
  }
}

} // namespace rezero
