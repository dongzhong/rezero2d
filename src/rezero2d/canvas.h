// Created by DONG Zhong on 2024/02/06.

#ifndef REZERO_CANVAS_H_
#define REZERO_CANVAS_H_

#include "rezero2d/bitmap.h"
#include "rezero2d/path.h"

namespace rezero {

class Canvas {
 public:
  Canvas();
  ~Canvas();

  void Begin(const std::shared_ptr<Bitmap>& bitmap);

  void End();

  void FillPath(const std::shared_ptr<Path>& path);

  void StrokePath(const std::shared_ptr<Path>& path);

 private:
  std::shared_ptr<Bitmap> bitmap_ = nullptr;

  REZERO_DISALLOW_COPY_ASSIGN_AND_MOVE(Canvas);
};

} // namespace rezero

#endif // REZERO_CANVAS_H_
