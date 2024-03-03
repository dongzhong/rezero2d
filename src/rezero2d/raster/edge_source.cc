// Created by DONG Zhong on 2024/02/28.

#include "rezero2d/raster/edge_source.h"

namespace rezero {

EdgeSource::EdgeSource(const EdgeTransform& transform, const Point* vertex_data,
                       const CommandType* cmd_data, std::size_t count)
    : transform_(transform), vertex_ptr_(vertex_data), cmd_ptr_(cmd_data),
      cmd_start_(cmd_data), cmd_end_(cmd_data + count) {}

bool EdgeSource::Begin(Point& p) {
  while (true) {
    if (cmd_ptr_ == cmd_end_) {
      return false;
    }

    auto cmd = *cmd_ptr_;
    ++cmd_ptr_;
    ++vertex_ptr_;

    if (cmd != CommandType::kMove) {
      continue;
    }

    transform_.Apply(p, *(vertex_ptr_ - 1));

    return true;
  }
}

} // namespace rezero
