// Created by DONG Zhong on 2024/02/06.

#ifndef REZERO_BITMAP_H_
#define REZERO_BITMAP_H_

#include <cstdint>

#include "rezero2d/base/macros.h"

namespace rezero {

class Bitmap {
 public:
  enum class Format : std::uint8_t {
    kARGB8888 = 0,
    // TODO:
  };

  Bitmap();
  ~Bitmap();

  void Init(std::uint32_t width, std::uint32_t height, Format format);

  Format GetFormat() const { return format_; }
  std::uint32_t GetWidth() const { return width_; }
  std::uint32_t GetHeight() const { return height_; }
  std::uint32_t GetStride() const { return width_ * GetFormatBytes(format_); }

 private:
  static std::uint32_t GetFormatBytes(Format format);

  Format format_;
  std::uint32_t width_ = 0;
  std::uint32_t height_ = 0;
  void* data_;

  REZERO_DISALLOW_COPY_ASSIGN_AND_MOVE(Bitmap);
};

} // namespace rezero

#endif // REZERO_BITMAP_H_
