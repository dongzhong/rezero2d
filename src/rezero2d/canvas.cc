// Created by DONG Zhong on 2024/02/06.

#include "rezero2d/canvas.h"

#include "rezero2d/base/logging.h"

namespace rezero {

Canvas::Canvas() = default;

Canvas::~Canvas() = default;

void Canvas::Begin(const std::shared_ptr<Bitmap>& bitmap) {
  REZERO_CHECK(!bitmap_);

  if (bitmap->flag_.test_and_set()) {
    REZERO_LOG(ERROR) << "Bitmap has been occupied.";
    return;
  }

  bitmap_ = bitmap;
}

void Canvas::End() {
  if (bitmap_) {
    bitmap_->flag_.clear();
  }

  bitmap_ = nullptr;
}

void Canvas::FillPath(const std::shared_ptr<Path>& path) {
  if (!bitmap_) {
    return;
  }
  // TODO:
}

void Canvas::StrokePath(const std::shared_ptr<Path>& path) {
  if (!bitmap_) {
    return;
  }

  // TODO:
}

} // namespace rezero
