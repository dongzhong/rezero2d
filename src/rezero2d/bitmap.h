// Created by DONG Zhong on 2024/02/06.

#ifndef REZERO_BITMAP_H_
#define REZERO_BITMAP_H_

#include <atomic>
#include <cstdint>
#include <memory>

#include "rezero2d/base/macros.h"
#include "rezero2d/data.h"
#include "rezero2d/format.h"

namespace rezero {

class Canvas;
enum class CodecType : std::uint8_t;

class Bitmap {
 public:
  Bitmap();
  ~Bitmap();

  void Init(std::uint32_t width, std::uint32_t height, Format format);

  Format GetFormat() const { return format_; }
  std::uint32_t GetWidth() const { return width_; }
  std::uint32_t GetHeight() const { return height_; }
  std::uint32_t GetStride() const { return stride_; }

  std::shared_ptr<Data> GetPixelData();

  std::shared_ptr<Data> EncodeAsFileData(CodecType type);

 private:
  Format format_;
  std::uint32_t width_ = 0;
  std::uint32_t height_ = 0;
  std::uint32_t stride_ = 0;
  void* data_;

  std::atomic_flag flag_ = ATOMIC_FLAG_INIT;

  friend class Canvas;

  REZERO_DISALLOW_COPY_ASSIGN_AND_MOVE(Bitmap);
};

} // namespace rezero

#endif // REZERO_BITMAP_H_
