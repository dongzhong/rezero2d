// Created by DONG Zhong on 2024/02/06.

#ifndef REZERO_BITMAP_H_
#define REZERO_BITMAP_H_

#include "rezero2d/base/macros.h"

namespace rezero {

class Bitmap {
 public:
  Bitmap();
  ~Bitmap();

 private:
  REZERO_DISALLOW_COPY_ASSIGN_AND_MOVE(Bitmap);
};

} // namespace rezero

#endif // REZERO_BITMAP_H_
