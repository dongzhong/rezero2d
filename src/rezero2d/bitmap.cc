// Created by DONG Zhong on 2024/02/06.

#include "rezero2d/bitmap.h"

#include "rezero2d/base/logging.h"

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
  data_ = std::malloc(width * height * GetFormatBytes(format));

  REZERO_CHECK(data_);
}

std::uint32_t Bitmap::GetFormatBytes(Format format) {
  switch (format) {
    case Format::kARGB8888: return 4;
    // TODO:
  }
}

} // namespace rezero
