// Created by DONG Zhong on 2024/02/12.

#ifndef REZERO_DATA_H_
#define REZERO_DATA_H_

#include <cstddef>
#include <string>

#include "rezero2d/base/macros.h"

namespace rezero {

class Data {
 public:
  Data();
  ~Data();

  void Init(std::size_t size, void* data);

  void* GetData() const { return data_; }

  bool SaveToFile(const std::string& file_path);

 private:
  std::size_t size_;
  void* data_;

  REZERO_DISALLOW_COPY_ASSIGN_AND_MOVE(Data);
};

} // namespace rezero

#endif // REZERO_DATA_H_
