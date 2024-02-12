project(rezero_2d_lib)

set(REZERO2D_SOURCE
  rezero2d/base/logging.cc
  rezero2d/base/logging.h
  rezero2d/base/macros.h

  rezero2d/bitmap.cc
  rezero2d/bitmap.h
  rezero2d/canvas.cc
  rezero2d/canvas.h
  rezero2d/geometry.cc
  rezero2d/geometry.h
  rezero2d/path.cc
  rezero2d/path.h)

add_library(rezero2d SHARED ${REZERO2D_SOURCE})

target_include_directories(rezero2d PUBLIC ${PROJECT_SOURCE_DIR})
