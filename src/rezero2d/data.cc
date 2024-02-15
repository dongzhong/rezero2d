// Created by DONG Zhong on 2024/02/12.

#include "rezero2d/data.h"

#include <cstdlib>
#include <cstring>
#include <fstream>

namespace rezero {

Data::Data() = default;

Data::~Data() {
  if (data_) {
    std::free(data_);
  }
}

void Data::Init(std::size_t size, void* data) {
  size_ = size;
  data_ = std::malloc(size);

  if (data) {
    std::memcpy(data_, data, size);
  } else {
    std::memset(data_, 0, size);
  }
}

bool Data::SaveToFile(const std::string& file_path) {
  std::ofstream ofs(file_path, std::ios::out | std::ios::binary);
  if (ofs) {
    if (size_ > 0) {
      ofs.write(static_cast<const char*>(data_), size_);
    }
    ofs.close();
    return true;
  }

  return false;
}

} // namespace rezero
