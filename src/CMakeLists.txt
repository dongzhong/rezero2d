project(rezero_2d_lib)

set(REZERO2D_SOURCE
  rezero2d/base/api.cc
  rezero2d/base/api.h
  rezero2d/base/logging.cc
  rezero2d/base/logging.h
  rezero2d/base/macros.h

  rezero2d/codec/bmp_codec.cc
  rezero2d/codec/bmp_codec.h

  rezero2d/raster/edge_builder.cc
  rezero2d/raster/edge_builder.h
  rezero2d/raster/edge_source.cc
  rezero2d/raster/edge_source.h
  rezero2d/raster/edge_storage.cc
  rezero2d/raster/edge_storage.h

  rezero2d/utils/int_operations.h

  rezero2d/bitmap.cc
  rezero2d/bitmap.h
  rezero2d/canvas.cc
  rezero2d/canvas.h
  rezero2d/codec.cc
  rezero2d/codec.h
  rezero2d/data.cc
  rezero2d/data.h
  rezero2d/format.cc
  rezero2d/format.h
  rezero2d/geometry.cc
  rezero2d/geometry.h
  rezero2d/path.cc
  rezero2d/path.h
)

add_library(rezero2d SHARED ${REZERO2D_SOURCE})

target_include_directories(rezero2d PUBLIC ${PROJECT_SOURCE_DIR})
