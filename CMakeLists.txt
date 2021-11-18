################################### Globals ####################################
cmake_minimum_required(VERSION 3.21)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(sixpp
  VERSION 0.0.1
  DESCRIPTION "Flamegraphs in your terminal")
#set(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)

set(ROOT "${CMAKE_CURRENT_LIST_DIR}")
list(APPEND CMAKE_MODULE_PATH "${ROOT}/cmake")

################################# Dependencies #################################
include(libsixel)
include_directories(${LIBSIXEL_INCLUDE})

set(PPROF_INCLUDE "${ROOT}")
set(PPROF_SRC "${ROOT}/proto/pprof.pb-c.c")

#################################### Target ####################################
include_directories(
  ${ROOT}/include
  ${ROOT}/proto)
add_executable(${PROJECT_NAME} ${ROOT}/src/sixpp.c ${PPROF_SRC})
target_link_libraries(${PROJECT_NAME}
  libprotobuf-c.a
  libsixel)
target_include_directories(${PROJECT_NAME} PRIVATE
  ${LIBSIXEL_ROOT}/include
  ${PPROF_INCLUDE})
