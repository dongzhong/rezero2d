project(example)

set(REZERO_ROOT_PATH ${PROJECT_SOURCE_DIR}/..)

set(EXAMPLE_SOURCE
  ${PROJECT_SOURCE_DIR}/example.cc)

add_executable(example ${EXAMPLE_SOURCE})

target_link_libraries(example PUBLIC rezero2d)
