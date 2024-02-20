// Created by DONG Zhong on 2024/02/15.

#include "rezero2d/format.h"

namespace rezero {

FormatInformation::FormatInformation(Format format) : format_(format) {
  switch (format) {
    case Format::kARGB8888: {
      bytes_per_pixel_ = 4;

      has_r_ = has_g_ = has_b_ = has_a_ = true;

      b_shift_ = 0;
      g_shift_ = 8;
      r_shift_ = 16;
      a_shift_ = 24;
      break;
    }
    // TODO:
  }
}

FormatInformation::~FormatInformation() = default;

} // namespace rezero
