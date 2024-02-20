// Created by DONG Zhong on 2024/02/13.

#ifndef REZERO_CODEC_BMP_CODEC_H_
#define REZERO_CODEC_BMP_CODEC_H_

#include "rezero2d/codec.h"

namespace rezero {

class BMPCodec : public Codec {
 public:
  BMPCodec();
  ~BMPCodec() override;

  std::shared_ptr<Data> EncodeToFileData(Format format, std::uint32_t width,
                                         std::uint32_t height, void* data) override;
};

} // namespace rezero

#endif // REZERO_CODEC_BMP_CODEC_H_
