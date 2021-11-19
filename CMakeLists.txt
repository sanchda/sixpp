# Notes
# This is some janky stuff.  It presumes a combination of weird dependencies are
# already local:
#  protobuf
#  anything required by libsixel
#  protoc-c

################################### Globals ####################################
cmake_minimum_required(VERSION 3.21)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(sixpp
  VERSION 0.0.1
  DESCRIPTION "Flamegraphs in your terminal")

set(ROOT "${CMAKE_CURRENT_LIST_DIR}")
set(SRC "${ROOT}/src")
list(APPEND CMAKE_MODULE_PATH "${ROOT}/cmake")


include(ExtendBuildTypes)

################################# Dependencies #################################
include(libsixel)
include_directories(${LIBSIXEL_INCLUDE})

set(PPROF_INCLUDE "${ROOT}")
set(PPROF_SRC "${ROOT}/proto/pprof.pb-c.c")

#################################### Tests #####################################
enable_testing()
add_subdirectory(test)

#################################### Target ####################################
include_directories(
  ${ROOT}/include
  ${ROOT}/proto)
add_executable(${PROJECT_NAME}
  ${SRC}/sixpp.c
  ${SRC}/tree.c
  ${SRC}/profile.c
  ${SRC}/canvas.c
  ${PPROF_SRC})
target_link_libraries(${PROJECT_NAME}
  /usr/lib/x86_64-linux-gnu/libprotobuf.a
  /usr/lib/x86_64-linux-gnu/libprotobuf-c.a
  libsixel)
target_include_directories(${PROJECT_NAME} PRIVATE
  ${LIBSIXEL_ROOT}/include
  ${PPROF_INCLUDE})
