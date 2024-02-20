// Created by DONG Zhong on 2024/02/15.

#ifndef REZERO_FORMAT_H_
#define REZERO_FORMAT_H_

#include <cstdint>

namespace rezero {

enum class Format : std::uint8_t {
  kARGB8888 = 0,
  // TODO:
};

class FormatInformation {
 public:
  FormatInformation(Format format);
  ~FormatInformation();

  std::uint32_t GetBytesPerPixel() const { return bytes_per_pixel_; }

  bool HasRChannel() const { return has_r_; }
  bool HasGChannel() const { return has_g_; }
  bool HasBChannel() const { return has_b_; }
  bool HasAChannel() const { return has_a_; }

  std::uint32_t GetRShift() const { return r_shift_; }
  std::uint32_t GetGShift() const { return g_shift_; }
  std::uint32_t GetBShift() const { return b_shift_; }
  std::uint32_t GetAShift() const { return a_shift_; }

 private:
  Format format_;
  std::uint32_t bytes_per_pixel_;

  bool has_r_;
  bool has_g_;
  bool has_b_;
  bool has_a_;

  std::uint32_t r_shift_;
  std::uint32_t g_shift_;
  std::uint32_t b_shift_;
  std::uint32_t a_shift_;
};

} // namespace rezero

#endif // REZERO_FORMAT_H_
