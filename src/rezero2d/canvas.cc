// Created by DONG Zhong on 2024/02/06.

#include "rezero2d/canvas.h"

#include "rezero2d/base/logging.h"

namespace rezero {

Canvas::Canvas() = default;

Canvas::~Canvas() {
  End();
}

bool Canvas::Begin(const std::shared_ptr<Bitmap>& bitmap) {
  REZERO_CHECK(!bitmap_);

  if (bitmap->flag_.test_and_set()) {
    REZERO_LOG(ERROR) << "Bitmap has been occupied.";
    return false;
  }

  bitmap_ = bitmap;

  return true;
}

void Canvas::End() {
  if (bitmap_) {
    bitmap_->flag_.clear();
  }

  bitmap_ = nullptr;
}

bool Canvas::FillPath(const std::shared_ptr<Path>& path) {
  if (!bitmap_) {
    return false;
  }

  // TODO:

  return true;
}

bool Canvas::StrokePath(const std::shared_ptr<Path>& path) {
  if (!bitmap_) {
    return false;
  }

  // TODO:

  return true;
}

} // namespace rezero
