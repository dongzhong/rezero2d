// Created by DONG Zhong on 2024/02/13.

#ifndef REZERO_CODEC_H_
#define REZERO_CODEC_H_

#include <cstdint>
#include <memory>

#include "rezero2d/data.h"
#include "rezero2d/format.h"

namespace rezero {

enum class CodecType : std::uint8_t {
  kDefault = 0,
  kBMP = 0,
};

class Codec {
 public:
  static std::shared_ptr<Codec> GetCodec(CodecType type);

  Codec() = default;
  virtual ~Codec() = default;

  virtual std::shared_ptr<Data> EncodeToFileData(Format format, std::uint32_t width,
                                                 std::uint32_t height, void* data) = 0;

 private:
  REZERO_DISALLOW_COPY_ASSIGN_AND_MOVE(Codec);
};

} // namespace rezero

#endif // REZERO_CODEC_H_
