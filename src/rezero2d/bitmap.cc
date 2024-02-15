// Created by DONG Zhong on 2024/02/06.

#include "rezero2d/bitmap.h"

#include "rezero2d/base/logging.h"
#include "rezero2d/codec.h"

namespace rezero {

Bitmap::Bitmap() = default;

Bitmap::~Bitmap() {
  if (data_) {
    std::free(data_);
  }
}

void Bitmap::Init(std::uint32_t width, std::uint32_t height, Format format) {
  REZERO_CHECK(width > 0 && height > 0);

  if (data_) {
    std::free(data_);
    data_ = nullptr;
  }

  format_ = format;
  width_ = width;
  height_ = height;

  FormatInformation format_info(format);
  stride_ = width * format_info.GetBytesPerPixel();
  data_ = std::malloc(width * height * format_info.GetBytesPerPixel());

  REZERO_CHECK(data_);
}

std::shared_ptr<Data> Bitmap::GetPixelData() {
  if (flag_.test_and_set()) {
    return nullptr;
  }

  auto data = std::make_shared<Data>();

  FormatInformation format_info(format_);
  data->Init(width_ * height_ * format_info.GetBytesPerPixel(), data_);

  flag_.clear();

  return data;
}



std::shared_ptr<Data> Bitmap::EncodeAsFileData(CodecType type) {
  if (flag_.test_and_set()) {
    REZERO_LOG(ERROR) << "Bitmap has been occupied.";
    return nullptr;
  }

  auto codec = Codec::GetCodec(type);
  auto data = codec->EncodeToFileData(format_, width_, height_, data_);

  flag_.clear();

  return data;
}

} // namespace rezero
